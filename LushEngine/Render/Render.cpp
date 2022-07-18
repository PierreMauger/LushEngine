#include "Render.hpp"

using namespace Lush;

Render::Render(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    this->_window = glfwCreateWindow(800, 600, "Lush Engine", NULL, NULL);

    if (this->_window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        this->_messageBus->sendMessage(Message(Packet(), BaseCommand::QUIT, Module::BROADCAST));
    }
    glfwMakeContextCurrent(this->_window);
}

Render::~Render()
{
    glfwTerminate();
}

void Render::run()
{
    while (this->_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

        if (glfwWindowShouldClose(this->_window)) {
            this->_running = false; // because it can sometimes send the quit message twice
            this->_messageBus->sendMessage(Message(Packet(), BaseCommand::QUIT, Module::BROADCAST));
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        glfwPollEvents();
        glfwSwapBuffers(this->_window);

        this->_messageBus->notify(Module::RENDER);
    }
}
