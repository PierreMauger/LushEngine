#include "Systems/Scene/SceneSystem.hpp"

using namespace Lush;

SceneSystem::SceneSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager) : ASystem(60.0f), _graphic(graphic)
{
    entityManager.addMaskCategory(MODEL_TAG);
    entityManager.addMaskCategory(BILLBOARD_TAG);
    entityManager.addMaskCategory(SKYBOX_TAG);
    entityManager.addMaskCategory(MAP_TAG);

    this->_cameraTransform.position = glm::vec3(5.0f, 5.0f, 15.0f);
    this->_cameraTransform.rotation = glm::vec3(-100.0f, -15.0f, 0.0f);
    this->_camera.forward.x = cos(glm::radians(this->_cameraTransform.rotation.x)) * cos(glm::radians(this->_cameraTransform.rotation.y));
    this->_camera.forward.y = sin(glm::radians(this->_cameraTransform.rotation.y));
    this->_camera.forward.z = sin(glm::radians(this->_cameraTransform.rotation.x)) * cos(glm::radians(this->_cameraTransform.rotation.y));

    Shapes::setupFrameBuffer(this->_buffer, this->_graphic->getWindowSize());
    this->_graphic->getFrameBuffers()["scene"] = this->_buffer;

    Shapes::setupSkyBox(this->_skybox);
    Shapes::setupBillboard(this->_billboard);
    Shapes::setupPlane(this->_grid);
}

SceneSystem::~SceneSystem()
{
    Shapes::deleteFrameBuffer(this->_buffer);
    Shapes::deleteBufferObject(this->_skybox);
    Shapes::deleteBufferObject(this->_billboard);
    Shapes::deleteBufferObject(this->_grid);
}

void SceneSystem::update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    this->_graphic->getRenderView().setAspectRatio(this->_graphic->getSceneViewPort().z / this->_graphic->getSceneViewPort().w);
    this->_graphic->getRenderView().update(this->_cameraTransform, this->_camera);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->handleMouse();

    this->drawSkybox(entityManager, componentManager);
    this->drawMap(entityManager, componentManager);
    this->drawModels(entityManager, componentManager);
    this->drawBillboards(entityManager, componentManager);
    this->drawGrid();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        this->_camera.forward.x = cos(glm::radians(this->_cameraTransform.rotation.x)) * cos(glm::radians(this->_cameraTransform.rotation.y));
        this->_camera.forward.y = sin(glm::radians(this->_cameraTransform.rotation.y));
        this->_camera.forward.z = sin(glm::radians(this->_cameraTransform.rotation.x)) * cos(glm::radians(this->_cameraTransform.rotation.y));
    }
}

void SceneSystem::drawModels(EntityManager &entityManager, ComponentManager &componentManager)
{
    this->_graphic->getRenderView().use("Model");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(MODEL_TAG)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Model model = componentManager.getComponent<Model>(id);

        this->_graphic->getRenderView().setModel(transform);
        if (this->_graphic->getModels().find(model.name) != this->_graphic->getModels().end())
            this->_graphic->getModels()[model.name].draw(this->_graphic->getRenderView().getShader());
    }
}

void SceneSystem::drawBillboards(EntityManager &entityManager, ComponentManager &componentManager)
{
    this->_graphic->getRenderView().use("Billboard");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(BILLBOARD_TAG)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Billboard billboard = componentManager.getComponent<Billboard>(id);

        this->_graphic->getRenderView().setBillboard(transform);
        glActiveTexture(GL_TEXTURE0);
        // TEMP : if texture not found, use black texture, but default texture should be better
        if (this->_graphic->getTextures().find(billboard.name) != this->_graphic->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_graphic->getTextures()[billboard.name]);
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        this->_graphic->getRenderView().getShader().setInt("tex", 0);
        glBindVertexArray(this->_billboard.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}

void SceneSystem::drawMap(EntityManager &entityManager, ComponentManager &componentManager)
{
    this->_graphic->getRenderView().use("Map");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(MAP_TAG)) {
        Map map = componentManager.getComponent<Map>(id);

        this->_graphic->getRenderView().getShader().setMat4("model", glm::mat4(1.0f));
        glActiveTexture(GL_TEXTURE0);
        if (this->_graphic->getTextures().find(map.name) != this->_graphic->getTextures().end())
            glBindTexture(GL_TEXTURE_2D, this->_graphic->getTextures()[map.name]);
        else
            glBindTexture(GL_TEXTURE_2D, 0);

        this->_graphic->getMap().draw();
    }
}

void SceneSystem::drawSkybox(EntityManager &entityManager, ComponentManager &componentManager)
{
    glDepthFunc(GL_LEQUAL);
    this->_graphic->getRenderView().use("Skybox");
    this->_graphic->getRenderView().setSkyBoxView();
    for (auto id : entityManager.getMaskCategory(SKYBOX_TAG)) {
        Cubemap cubemap = componentManager.getComponent<Cubemap>(id);

        if (this->_graphic->getSkyboxes().find(cubemap.name) != this->_graphic->getSkyboxes().end()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->_graphic->getSkyboxes()[cubemap.name]);
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
