#include "Systems/Scene/SceneSystem.hpp"

using namespace Lush;

SceneSystem::SceneSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
{
    this->_cameraTransform.position = glm::vec3(10.0f, 5.0f, 15.0f);
    this->_cameraTransform.rotation = glm::vec3(-130.0f, -15.0f, 0.0f);
    this->_camera.forward.x = (float)(cos(glm::radians(this->_cameraTransform.rotation.x)) * cos(glm::radians(this->_cameraTransform.rotation.y)));
    this->_camera.forward.y = (float)(sin(glm::radians(this->_cameraTransform.rotation.y)));
    this->_camera.forward.z = (float)(sin(glm::radians(this->_cameraTransform.rotation.x)) * cos(glm::radians(this->_cameraTransform.rotation.y)));
    this->_camera.far = 1000.0f;

    Shapes::setupFrameBuffer(this->_buffer, this->_graphic->getWindowSize());
    this->_graphic->getFrameBuffers()["scene"] = this->_buffer;

    Shapes::setupSkyBox(this->_skybox);
    Shapes::setupBillboard(this->_billboard);
    Shapes::setupPlane(this->_grid);
    Shapes::setupCube(this->_cameraFrustum);

    this->generatePerlinTexture();
}

SceneSystem::~SceneSystem()
{
    Shapes::deleteFrameBuffer(this->_buffer);
    Shapes::deleteBufferObject(this->_skybox);
    Shapes::deleteBufferObject(this->_billboard);
    Shapes::deleteBufferObject(this->_grid);
}

void SceneSystem::update(EntityManager &entityManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    this->_graphic->getRenderView().setAspectRatio(this->_graphic->getSceneViewPort().z / this->_graphic->getSceneViewPort().w);
    this->_graphic->getRenderView().update(this->_cameraTransform, this->_camera);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->handleMouse();

    this->drawSkybox(entityManager);
    this->drawMap(entityManager);
    this->drawModels(entityManager);
    this->drawBillboards(entityManager);
    this->drawCameraFrustum(entityManager);
    this->drawGrid();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneSystem::generatePerlinTexture()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10000);
    int seed = dis(gen);

    glGenTextures(1, &this->_perlinTexture);
    glBindTexture(GL_TEXTURE_2D, this->_perlinTexture);

    int width = 256;
    int height = 256;
    auto *data = new unsigned char[width * height * 3];

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float x = (float)j / (float)width;
            float y = (float)i / (float)height;
            float value = glm::perlin(glm::vec3(x * 10.0f, y * 10.0f, seed));
            int index = (i * width + j) * 3;
            data[index] = (unsigned char)(value * 128.0f + 127);
            data[index + 1] = (unsigned char)(value * 128.0f + 127);
            data[index + 2] = (unsigned char)(value * 128.0f + 127);
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    delete[] data;
}

void SceneSystem::handleMouse()
{
    if (this->_graphic->getSceneMovement()) {
        if (this->_graphic->getMouseButton() == 0)
            this->_graphic->getRenderView().rotate(this->_cameraTransform, this->_graphic->getMouseOffset());
        if (this->_graphic->getMouseButton() == 1)
            this->_cameraTransform.position += this->_camera.forward * this->_graphic->getMouseOffset().y * 0.05f;
        if (this->_graphic->getMouseButton() == 2) {
            glm::vec3 cameraRight = glm::normalize(glm::cross(this->_camera.forward, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, this->_camera.forward));
            this->_cameraTransform.position -= cameraRight * this->_graphic->getMouseOffset().x * 0.02f;
            this->_cameraTransform.position -= cameraUp * this->_graphic->getMouseOffset().y * 0.02f;
        }
        if (this->_graphic->getMouseButton() == 3)
            this->generatePerlinTexture();
        this->_camera.forward.x = (float)(cos(glm::radians(this->_cameraTransform.rotation.x)) * cos(glm::radians(this->_cameraTransform.rotation.y)));
        this->_camera.forward.y = (float)(sin(glm::radians(this->_cameraTransform.rotation.y)));
        this->_camera.forward.z = (float)(sin(glm::radians(this->_cameraTransform.rotation.x)) * cos(glm::radians(this->_cameraTransform.rotation.y)));
    }
}

void SceneSystem::drawModels(EntityManager &entityManager)
{
    this->_graphic->getRenderView().use("Model");
    this->_graphic->getRenderView().setView();
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Model>())
            continue;
        Transform transform = entity.getComponent<Transform>();
        Model model = entity.getComponent<Model>();

        this->_graphic->getRenderView().setModel(transform);
        if (this->_resourceManager->getModels().find(model.name) != this->_resourceManager->getModels().end())
            this->_resourceManager->getModels()[model.name].draw(this->_graphic->getRenderView().getShader());
    }
}

void SceneSystem::drawBillboards(EntityManager &entityManager)
{
    this->_graphic->getRenderView().use("Billboard");
    this->_graphic->getRenderView().setView();
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Billboard>())
            continue;
        Transform transform = entity.getComponent<Transform>();
        Billboard billboard = entity.getComponent<Billboard>();

        this->_graphic->getRenderView().setBillboard(transform);
        glActiveTexture(GL_TEXTURE0);
        if (this->_resourceManager->getTextures().find(billboard.name) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[billboard.name].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("tex", 0);
        glBindVertexArray(this->_billboard.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

void SceneSystem::drawMap(EntityManager &entityManager)
{
    this->_graphic->getRenderView().use(this->_graphic->isWireframe() ? "MapWireframe" : "Map");
    this->_graphic->getRenderView().setView();
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (!entity.hasComponent<Map>())
            continue;
        Map map = entity.getComponent<Map>();

        // TODO : clean this and add it to render system
        this->_graphic->getRenderView().getShader().setMat4("model", glm::mat4(1.0f));
        glActiveTexture(GL_TEXTURE0);
        if (this->_resourceManager->getTextures().find(map.heightMap) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[map.heightMap].getId());
        else
            glBindTexture(GL_TEXTURE_2D, this->_perlinTexture);
        this->_graphic->getRenderView().getShader().setInt("heightMap", 0);
        glActiveTexture(GL_TEXTURE1);
        if (this->_resourceManager->getTextures().find(map.diffuseTexture) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[map.diffuseTexture].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("diffuseTexture", 1);
        glActiveTexture(GL_TEXTURE2);
        if (this->_resourceManager->getTextures().find(map.normalTexture) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[map.normalTexture].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("normalTexture", 2);
        glActiveTexture(GL_TEXTURE3);
        if (this->_resourceManager->getTextures().find(map.diffuseTexture2) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[map.diffuseTexture2].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("diffuseTexture2", 3);
        glActiveTexture(GL_TEXTURE4);
        if (this->_resourceManager->getTextures().find(map.diffuseTexture3) != this->_resourceManager->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_resourceManager->getTextures()[map.diffuseTexture3].getId());
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("diffuseTexture3", 4);
        this->_resourceManager->getMapMesh().draw();
    }
}

void SceneSystem::drawSkybox(EntityManager &entityManager)
{
    glDepthFunc(GL_LEQUAL);
    this->_graphic->getRenderView().use("Skybox");
    this->_graphic->getRenderView().setSkyBoxView();
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (!entity.hasComponent<Cubemap>())
            continue;
        Cubemap cubeMap = entity.getComponent<Cubemap>();

        if (this->_resourceManager->getSkyBoxes().find(cubeMap.name) != this->_resourceManager->getSkyBoxes().end()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->_resourceManager->getSkyBoxes()[cubeMap.name].getId());
            this->_graphic->getRenderView().getShader().setInt("skybox", 0);
            glBindVertexArray(this->_skybox.vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
    glDepthFunc(GL_LESS);
}

void SceneSystem::drawGrid()
{
    this->_graphic->getRenderView().use("Grid");
    this->_graphic->getRenderView().setView();
    glEnable(GL_BLEND);
    glBindVertexArray(this->_grid.vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}

void SceneSystem::drawCameraFrustum(EntityManager &entityManager)
{
    if (entityManager.getEntities().find(this->_graphic->getSelectedEntity()) == entityManager.getEntities().end())
        return;
    Entity &entity = entityManager.getEntity(this->_graphic->getSelectedEntity());
    if (!entity.hasComponent<Transform>() || !entity.hasComponent<Camera>())
        return;

    this->_graphic->getRenderView().use("CameraFrustum");
    this->_graphic->getRenderView().setView();

    Transform transform = entity.getComponent<Transform>();
    Camera camera = entity.getComponent<Camera>();
    float aspectRatio = this->_graphic->getGameViewPort().z / this->_graphic->getGameViewPort().w;
    glm::mat4 view = glm::lookAt(transform.position, transform.position + camera.forward, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 frustum = glm::inverse(glm::perspective(glm::radians(camera.fov), aspectRatio, camera.near, camera.far) * view);
    this->_graphic->getRenderView().getShader().setMat4("frustum", frustum);

    glBindVertexArray(this->_cameraFrustum.vao);
    glDrawArrays(GL_LINES, 0, 24);
    glBindVertexArray(0);
}
