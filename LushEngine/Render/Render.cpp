#include "Render.hpp"

using namespace Lush;

Render::Render(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
    if (!glfwInit())
        throw std::runtime_error("GLFW failed to initialize");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    this->_window = glfwCreateWindow(800, 600, "Lush Engine", NULL, NULL);

    if (this->_window == NULL)
        std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(this->_window);

    if (glewInit() != GLEW_OK)
        std::runtime_error("GLEW failed to initialize");
    if (!GLEW_VERSION_2_1)
        throw std::runtime_error("GLEW does not support OpenGL 2.1");
    this->_camera = std::make_unique<Camera>();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

Render::~Render()
{
    if (this->_window != NULL)
        glfwDestroyWindow(this->_window);
    glfwTerminate();
}

void Render::run()
{
    while (this->_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        this->draw();
        if (glfwWindowShouldClose(this->_window)) {
            this->_running = false; // because it can sometimes send the quit message twice
            this->sendMessage(Message(Packet(), BaseCommand::QUIT, Module::BROADCAST));
        }
        this->_messageBus->notify(Module::RENDER, this->_functionList);
    }
}

void Render::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    glfwPollEvents();
    glfwSwapBuffers(this->_window);
    this->_camera->getShader().use();
    for (auto &[key, object] : this->_objects)
        object->draw(*this->_camera);
}
