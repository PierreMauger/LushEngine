#include "Systems/Scene/SceneSystem.hpp"

using namespace Lush;

SceneSystem::SceneSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager)
{
    this->_graphic = graphic;
    entityManager.addMaskCategory(this->_modelTag);
    entityManager.addMaskCategory(this->_billboardTag);
    entityManager.addMaskCategory(this->_skyboxTag);
    glm::vec2 windowSize = this->_graphic->getWindowSize();

    glGenFramebuffers(1, &this->_buffer.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);
    glGenTextures(1, &this->_buffer.texture);
    glBindTexture(GL_TEXTURE_2D, this->_buffer.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowSize.x, windowSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_buffer.texture, 0);
    glGenRenderbuffers(1, &this->_buffer.depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->_buffer.depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowSize.x, windowSize.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->_buffer.depthbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->_graphic->getFrameBuffers().push_back(this->_buffer);

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

SceneSystem::~SceneSystem()
{
}

void SceneSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    this->_graphic->getRenderView().setAspectRatio(this->_graphic->getSceneViewPort().z / this->_graphic->getSceneViewPort().w);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->_graphic->getRenderView().use("Camera");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(this->_modelTag)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        Model model = componentManager.getComponent<Model>(id);

        this->_graphic->getRenderView().setModel(transform);
        if (this->_graphic->getModels().find(model.id) != this->_graphic->getModels().end())
            this->_graphic->getModels()[model.id].draw(this->_graphic->getRenderView().getShader());
    }

    this->_graphic->getRenderView().use("Billboard");
    this->_graphic->getRenderView().setView();
    for (auto id : entityManager.getMaskCategory(this->_billboardTag)) {
        Transform transform = componentManager.getComponent<Transform>(id);
        BillBoard billBoard = componentManager.getComponent<BillBoard>(id);

        this->_graphic->getRenderView().setBillboard(transform);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, billBoard.textureId);
        this->_graphic->getRenderView().getShader().setInt("tex", 0);
        glBindVertexArray(this->_billboardVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    // INFO : dont draw to differenciate scene and game
    // glDepthFunc(GL_LEQUAL);
    // this->_graphic->getRenderView().use("Skybox");
    // this->_graphic->getRenderView().setSkyBoxView();
    // for (auto id : entityManager.getMaskCategory(this->_skyboxTag)) {
    //     CubeMap cubeMap = componentManager.getComponent<CubeMap>(id);

    //     if (this->_graphic->getSkyboxes().find(cubeMap.id) != this->_graphic->getSkyboxes().end()) {
    //         this->_graphic->getRenderView().setSkyBox(cubeMap);
    //         glActiveTexture(GL_TEXTURE0);
    //         glBindTexture(GL_TEXTURE_CUBE_MAP, this->_graphic->getSkyboxes()[cubeMap.id]);
    //         this->_graphic->getRenderView().getShader().setInt("skybox", 0);
    //         glBindVertexArray(this->_skyboxVAO);
    //         glDrawArrays(GL_TRIANGLES, 0, 36);
    //         glBindVertexArray(0);
    //     }
    // }
    // glDepthFunc(GL_LESS);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
