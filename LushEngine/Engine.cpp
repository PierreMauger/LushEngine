#include "Engine.hpp"

using namespace Lush;

Engine::Engine(bool isEditor) : _isEditor(isEditor)
{
    this->_graphic = std::make_shared<Graphic>(1280, 720, "Lush Engine");
    this->_resourceManager = std::make_shared<ResourceManager>();
    this->_resourceManager->initScriptDomain(isEditor ? "libs" : "Data");
    this->_isEditor ? this->_resourceManager->loadEditor() : this->_resourceManager->loadGame();

    this->_graphic->getRenderView().setShaders(this->_resourceManager->getShaders());

    this->_ecs.getSystemManager().bindSystem(std::make_unique<ScriptSystem>(this->_graphic, this->_resourceManager));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<CameraSystem>(this->_graphic));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<RenderSystem>(this->_graphic, this->_resourceManager));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<PhysicSystem>(this->_graphic, this->_resourceManager));
    if (this->_isEditor) {
        this->_ecs.getSystemManager().bindSystem(std::make_unique<SceneSystem>(this->_graphic, this->_resourceManager));
        this->_ecs.getSystemManager().bindSystem(std::make_unique<PickingSystem>(this->_graphic, this->_resourceManager));
        this->_ecs.getSystemManager().bindSystem(std::make_unique<GUISystem>(this->_graphic, this->_resourceManager));
        this->_ecs.getSystemManager().bindSystem(std::make_unique<FileWatcherSystem>(this->_graphic, this->_resourceManager));
    } else {
        this->_ecs.getSystemManager().bindSystem(std::make_unique<GameSystem>(this->_graphic, this->_resourceManager));
    }

    if (!this->_isEditor && this->_resourceManager->getScenes().find("main") != this->_resourceManager->getScenes().end()) {
        this->_resourceManager->getScenes()["main"].setScene(this->_ecs.getEntityManager());
        this->_resourceManager->setActiveScene("main");
    } else {
        this->_resourceManager->getScenes().begin()->second.setScene(this->_ecs.getEntityManager());
        this->_resourceManager->setActiveScene(this->_resourceManager->getScenes().begin()->first);
    }
}

void Engine::run()
{
    while (!glfwWindowShouldClose(this->_graphic->getWindow())) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        this->updateMouse();

        this->_ecs.getSystemManager().updateSystems(this->_ecs.getEntityManager(), this->_deltaTime);

        glfwSwapBuffers(this->_graphic->getWindow());
        glfwPollEvents();
        this->_deltaTime = (float)glfwGetTime() - this->_lastTime;
        this->_lastTime = (float)glfwGetTime();
    }
}

void Engine::updateMouse()
{
    double x, y;
    glfwGetCursorPos(this->_graphic->getWindow(), &x, &y);
    this->_graphic->setMouseOffset({x, y});
    this->_graphic->setMousePosition({x, y});
}
