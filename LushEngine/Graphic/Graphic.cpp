#include "Graphic.hpp"

using namespace Lush;

// TODO loader
std::string loadFile2(std::string fileName)
{
    std::ifstream file;
    std::string buffer;
    std::stringstream stream;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(fileName);
        stream << file.rdbuf();
        file.close();
        buffer = stream.str();
    } catch (std::ifstream::failure &e) {
        throw std::runtime_error("File loading error: " + fileName);
    }
    return buffer;
}

Graphic::Graphic() : _camera(1280, 720)
{
    this->setupWindow();

    this->_models[0] = Model(loadFile2("Resources/Models/Cube.dae"), std::map<std::string, unsigned int>());
    this->_shaders["Camera"] = Shader(loadFile2("Resources/Shaders/camera.vs"), loadFile2("Resources/Shaders/camera.fs"));

    this->_camera.setShaders(this->_shaders);
}

void Graphic::setupWindow()
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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glfwSwapInterval(1); // Enable vsync
}

std::shared_ptr<GLFWwindow> Graphic::getWindow()
{
    return this->_window;
}

std::map<std::string, Shader> &Graphic::getShaders()
{
    return this->_shaders;
}

std::map<std::size_t, Model> &Graphic::getModels()
{
    return this->_models;
}

RenderView &Graphic::getCamera()
{
    return this->_camera;
}
