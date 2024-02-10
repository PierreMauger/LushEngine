#include "Engine.hpp"

using namespace Lush;

Engine::Engine(bool isEditor) : _isEditor(isEditor)
{
    this->_graphic = std::make_shared<Graphic>(1280, 720, "Lush Engine");
    // init resources
    this->_resourceManager = std::make_shared<ResourceManager>(isEditor ? "libs" : "Data");
    this->_isEditor ? this->_resourceManager->loadEditor() : this->_resourceManager->loadGame();
    auto &scenes = this->_resourceManager->getScenes();
    std::string mainSceneName = scenes.contains("main") ? "main" : scenes.begin()->first;
    this->_resourceManager->setActiveScene(mainSceneName);

    // init graphic
    this->_graphic->setLogo(this->_resourceManager->getLogo());
    this->_graphic->getRenderView().setShaders(this->_resourceManager->getShaders());

    // init ecs
    this->_ecs.getSystemManager().bindSystem<PhysicSystem>(this->_graphic, this->_resourceManager);
    this->_ecs.getSystemManager().bindSystem<ScriptSystem>(this->_graphic, this->_resourceManager);
    this->_ecs.getSystemManager().bindSystem<CameraSystem>(this->_graphic, this->_resourceManager);
    this->_ecs.getSystemManager().bindSystem<RenderSystem>(this->_graphic, this->_resourceManager);
#ifdef EDITOR_MODE
    this->_ecs.getSystemManager().bindSystem<SceneSystem>(this->_graphic, this->_resourceManager);
    this->_ecs.getSystemManager().bindSystem<PickingSystem>(this->_graphic, this->_resourceManager);
    this->_ecs.getSystemManager().bindSystem<GUISystem>(this->_graphic, this->_resourceManager);
    this->_ecs.getSystemManager().bindSystem<FileWatcherSystem>(this->_graphic, this->_resourceManager);

    this->_ecs.getEntityManager() = this->_resourceManager->getActiveScene().getEntityManager();
    // this->_ecs.getEntityManager() = std::make_shared<EntityManager>(this->_resourceManager->getActiveScene().getEntityManager());
#else
    this->_ecs.getSystemManager().bindSystem<GameSystem>(this->_graphic, this->_resourceManager);

    this->_ecs.getEntityManager()->clone(*this->_resourceManager->getActiveScene().getEntityManager());
    this->_resourceManager->initScriptInstances(this->_ecs.getEntityManager());
    this->_resourceManager->initPhysicInstances(this->_ecs.getEntityManager());
    this->_graphic->setRunning(true);
#endif
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
