#include "Engine.hpp"

using namespace Lush;

Engine::Engine()
{
    this->_entityManager = EntityManager();
    this->_componentManager = ComponentManager();
    this->_systemManager = SystemManager();

    this->initWindow();

    this->_systemManager.bindSystem(std::make_shared<RenderSystem>(this->_window));
    this->_systemManager.bindSystem(std::make_shared<GUISystem>(this->_window));
    this->_systemManager.bindSystem(std::make_shared<ControlSystem>(this->_window));

    this->_componentManager.bindComponent<Position>();
    this->_componentManager.bindComponent<Velocity>();
    this->_componentManager.bindComponent<ModelID>();
}

void Engine::initWindow()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");
    this->_window = std::shared_ptr<GLFWwindow>(glfwCreateWindow(1280, 720, "Lush Engine", NULL, NULL), glfwDestroyWindow);
    if (!this->_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(this->_window.get());

    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Failed to initialize GLEW");

    // glfwSwapInterval(1); // Enable vsync
}

void Engine::run()
{
    while (!glfwWindowShouldClose(this->_window.get())) {
        this->clear();
        this->_systemManager.updateSystems(this->_componentManager, this->_entityManager);
        glfwSwapBuffers(this->_window.get());
        this->draw();
    }
}

void Engine::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Engine::draw()
{
    glfwPollEvents();
}
