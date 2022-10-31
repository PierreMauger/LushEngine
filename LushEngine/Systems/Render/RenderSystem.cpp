#include "Systems/Render/RenderSystem.hpp"

using namespace Lush;

RenderSystem::RenderSystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;

    static const float skyboxVertices[] = {
        // vec3 vertexPos
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    glGenVertexArrays(1, &this->_skyBoxVAO);
    glGenBuffers(1, &this->_skyBoxVBO);
    glBindVertexArray(this->_skyBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_skyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	static const float billboardVertices[] = {
        // vec2 vertexPos, vec2 texCoords
        -1.0f,  1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f,
    };
    glGenVertexArrays(1, &this->_billVAO);
    glGenBuffers(1, &this->_billVBO);
    glBindVertexArray(this->_billVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_billVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(billboardVertices), &billboardVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    std::size_t renderable = (ComponentType::TRANSFORM | ComponentType::MODEL);
    std::size_t skybox = (ComponentType::CUBEMAP);
    std::size_t bill = (ComponentType::TRANSFORM | ComponentType::BILLBOARD);

    this->_graphic->getCamera().use("Camera");
    for (auto &id : entityManager.getMaskCategory(renderable)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Model model = componentManager.getComponent<Model>(id);

        this->_graphic->getCamera().setOnModel(transform);
        if (this->_graphic->getModels().find(model.id) != this->_graphic->getModels().end())
            this->_graphic->getModels()[model.id].draw(this->_graphic->getCamera().getShader());
    }

    this->_graphic->getCamera().use("Billboard");
    for (auto &id : entityManager.getMaskCategory(bill)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        BillBoard billBoard = componentManager.getComponent<BillBoard>(id);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, billBoard.textureId);
        this->_graphic->getCamera().getShader().setInt("myTextureSampler", 0);

        this->_graphic->getCamera().setOnBillboard(transform);
        glBindVertexArray(this->_billVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    glDepthFunc(GL_LEQUAL);
    this->_graphic->getCamera().use("Skybox");
    this->_graphic->getCamera().setSkyBox();
    for (auto &id : entityManager.getMaskCategory(skybox)) {
        CubeMap cubeMap = componentManager.getComponent<CubeMap>(id);

        if (this->_graphic->getSkyboxes().find(cubeMap.id) != this->_graphic->getSkyboxes().end()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->_graphic->getSkyboxes()[cubeMap.id]);
            this->_graphic->getCamera().getShader().setInt("skybox", 0);
            glm::mat4 model = glm::mat4(1.0f);
            if (cubeMap.rotationSpeed != 0.0f)
                model = glm::rotate(model, glm::radians((float)glfwGetTime() * cubeMap.rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
            this->_graphic->getCamera().getShader().setMat4("model", model);
            this->_graphic->getCamera().getShader().setVec3("color", cubeMap.color);
            glBindVertexArray(this->_skyBoxVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
    glDepthFunc(GL_LESS);
}
