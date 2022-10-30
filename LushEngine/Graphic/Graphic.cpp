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

unsigned int loadTexture(std::string name)
{
    unsigned int textureID;

    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        stbi_image_free(data);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
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

    this->_textures["Crate.png"] = loadTexture("Resources/Textures/Crate.png");
    this->_textures["Crate_specular.png"] = loadTexture("Resources/Textures/Crate_specular.png");
    this->_textures["Crate_emission.png"] = loadTexture("Resources/Textures/Crate_emission.png");

    this->_models[0] = RenderModel(loadFile2("Resources/Models/Cube.dae"), this->_textures);
    this->_models[1] = RenderModel(loadFile2("Resources/Models/Crate.dae"), this->_textures);

    this->_skyboxes[0] = loadCubemap({"Resources/Skybox/right.jpg", "Resources/Skybox/left.jpg", "Resources/Skybox/top.jpg", "Resources/Skybox/bottom.jpg",
                                      "Resources/Skybox/front.jpg", "Resources/Skybox/back.jpg"});

    this->_shaders["Camera"] = Shader(loadFile2("Resources/Shaders/camera.vs"), loadFile2("Resources/Shaders/camera.fs"));
    this->_shaders["Picking"] = Shader(loadFile2("Resources/Shaders/camera.vs"), loadFile2("Resources/Shaders/picking.fs"));
    this->_shaders["Outline"] = Shader(loadFile2("Resources/Shaders/outline.vs"), loadFile2("Resources/Shaders/outline.fs"));
    this->_shaders["Skybox"] = Shader(loadFile2("Resources/Shaders/skybox.vs"), loadFile2("Resources/Shaders/skybox.fs"));
    this->_shaders["Billboard"] = Shader(loadFile2("Resources/Shaders/billboard.vs"), loadFile2("Resources/Shaders/billboard.fs"));
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

std::map<std::string, unsigned int> &Graphic::getTextures()
{
    return this->_textures;
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
