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

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}

Graphic::Graphic() : _camera(1280, 720)
{
    this->setupWindow();

    this->_models[0] = RenderModel(loadFile2("Resources/Models/Cube.dae"), std::map<std::string, unsigned int>());
    this->_shaders["Camera"] = Shader(loadFile2("Resources/Shaders/camera.vs"), loadFile2("Resources/Shaders/camera.fs"));
    this->_shaders["Picking"] = Shader(loadFile2("Resources/Shaders/camera.vs"), loadFile2("Resources/Shaders/picking.fs"));
    this->_shaders["Outline"] = Shader(loadFile2("Resources/Shaders/outline.vs"), loadFile2("Resources/Shaders/outline.fs"));
    this->_shaders["Skybox"] = Shader(loadFile2("Resources/Shaders/skybox.vs"), loadFile2("Resources/Shaders/skybox.fs"));

    this->_camera.setShaders(this->_shaders);

    this->_skyboxes[0] = loadCubemap({
        "Resources/Skybox/right.jpg",
        "Resources/Skybox/left.jpg",
        "Resources/Skybox/top.jpg",
        "Resources/Skybox/bottom.jpg",
        "Resources/Skybox/front.jpg",
        "Resources/Skybox/back.jpg"
    });
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

std::map<std::size_t, RenderModel> &Graphic::getModels()
{
    return this->_models;
}

std::map<std::size_t, unsigned int> &Graphic::getSkyboxes()
{
    return this->_skyboxes;
}

RenderView &Graphic::getCamera()
{
    return this->_camera;
}

void Graphic::setMouseMovement(bool mouseMovement)
{
    this->_mouseMovement = mouseMovement;
}

bool Graphic::getMouseMovement()
{
    return this->_mouseMovement;
}

void Graphic::setMousePosition(glm::vec2 mousePosition)
{
    this->_mouseLastPosition = this->_mousePosition;
    this->_mousePosition = mousePosition;
}

void Graphic::setMouseOffset(glm::vec2 mousePosition)
{
    this->_mouseOffset.x = mousePosition.x - this->_mouseLastPosition.x;
    this->_mouseOffset.y = this->_mouseLastPosition.y - mousePosition.y;
    this->_mouseLastPosition = mousePosition;
}

glm::vec2 Graphic::getMousePosition()
{
    return this->_mousePosition;
}

glm::vec2 Graphic::getMouseOffset()
{
    return this->_mouseOffset;
}
