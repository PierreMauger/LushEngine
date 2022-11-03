#include "Systems/Render/RenderSystem.hpp"

using namespace Lush;

RenderSystem::RenderSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager)
{
    this->_graphic = graphic;
    entityManager.addMaskCategory(this->_modelTag);
    entityManager.addMaskCategory(this->_billboardTag);
    entityManager.addMaskCategory(this->_skyboxTag);

    glGenVertexArrays(1, &this->_skyboxVAO);
    glGenBuffers(1, &this->_skyboxVBO);
    glBindVertexArray(this->_skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glGenVertexArrays(1, &this->_billboardVAO);
    glGenBuffers(1, &this->_billboardVBO);
    glBindVertexArray(this->_billboardVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_billboardVBO);
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
    this->_graphic->getCamera().use("Camera");
    this->_graphic->getCamera().setView();
    for (auto &id : entityManager.getMaskCategory(this->_modelTag)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Model model = componentManager.getComponent<Model>(id);

        this->_graphic->getCamera().setModel(transform);
        if (this->_graphic->getModels().find(model.id) != this->_graphic->getModels().end())
            this->_graphic->getModels()[model.id].draw(this->_graphic->getCamera().getShader());
    }

    this->_graphic->getCamera().use("Billboard");
    this->_graphic->getCamera().setView();
    for (auto &id : entityManager.getMaskCategory(this->_billboardTag)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        BillBoard billBoard = componentManager.getComponent<BillBoard>(id);

        this->_graphic->getCamera().setBillboard(transform);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, billBoard.textureId);
        this->_graphic->getCamera().getShader().setInt("tex", 0);
        glBindVertexArray(this->_billboardVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    glDepthFunc(GL_LEQUAL);
    this->_graphic->getCamera().use("Skybox");
    this->_graphic->getCamera().setSkyBoxView();
    for (auto &id : entityManager.getMaskCategory(this->_skyboxTag)) {
        CubeMap cubeMap = componentManager.getComponent<CubeMap>(id);

        if (this->_graphic->getSkyboxes().find(cubeMap.id) != this->_graphic->getSkyboxes().end()) {
            this->_graphic->getCamera().setSkyBox(cubeMap);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->_graphic->getSkyboxes()[cubeMap.id]);
            this->_graphic->getCamera().getShader().setInt("skybox", 0);
            glBindVertexArray(this->_skyboxVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }
    }
    glDepthFunc(GL_LESS);
}
