#include "Engine.hpp"

using namespace Lush;

Engine::Engine()
{
    this->_graphic = std::make_shared<Graphic>(1280, 720, "Lush Engine");

    this->_ecs.getComponentManager().bindComponent<Transform>();
    this->_ecs.getComponentManager().bindComponent<Velocity>();
    this->_ecs.getComponentManager().bindComponent<Model>();
    this->_ecs.getComponentManager().bindComponent<Camera>();
    this->_ecs.getComponentManager().bindComponent<Light>();
    this->_ecs.getComponentManager().bindComponent<Cubemap>();
    this->_ecs.getComponentManager().bindComponent<Billboard>();
    this->_ecs.getComponentManager().bindComponent<Map>();

    this->_ecs.getSystemManager().bindSystem(std::make_unique<ScriptSystem>(this->_graphic, this->_ecs.getEntityManager()));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<CameraSystem>(this->_graphic, this->_ecs.getEntityManager()));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<RenderSystem>(this->_graphic, this->_ecs.getEntityManager()));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<SceneSystem>(this->_graphic, this->_ecs.getEntityManager()));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<PickingSystem>(this->_graphic, this->_ecs.getEntityManager()));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<GUISystem>(this->_graphic));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<FileWatcherSystem>(this->_graphic, this->_ecs.getEntityManager()));

    this->_graphic->getScene()->setScene(this->_ecs.getEntityManager(), this->_ecs.getComponentManager());
}

void Engine::run()
{
    std::streambuf *oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(this->_graphic->getStringStream().rdbuf());

    while (!glfwWindowShouldClose(this->_graphic->getWindow())) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        this->updateMouse();

        this->_ecs.getSystemManager().updateSystems(this->_ecs.getEntityManager(), this->_ecs.getComponentManager(), this->_deltaTime);

        glfwSwapBuffers(this->_graphic->getWindow());
        glfwPollEvents();
        this->_deltaTime = glfwGetTime() - this->_lastTime;
        this->_lastTime = glfwGetTime();
    }
    std::cout.rdbuf(oldCoutStreamBuf);
}

void Engine::updateMouse()
{
    double x, y;
    glfwGetCursorPos(this->_graphic->getWindow(), &x, &y);
    if (this->_graphic->getMouseMovement() || this->_graphic->getSceneMovement())
        this->_graphic->setMouseOffset(glm::vec2(x, y));
    if (!this->_graphic->getMouseMovement())
        this->_graphic->setMousePosition(glm::vec2(x, y));
}
