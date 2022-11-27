#include "Graphic/Graphic.hpp"

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
        GLenum format = GL_RGB;
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

Graphic::Graphic(int sizeX, int sizeY, std::string title) : _renderView(sizeX / sizeY)
{
    this->setGLFWContext(sizeX, sizeY, title);

    this->_mousePosition = glm::vec2(sizeX / 2, sizeY / 2);
    this->_gameViewPort = glm::vec4(0.0f, 0.0f, sizeX, sizeY);
    this->_sceneViewPort = glm::vec4(0.0f, 0.0f, sizeX, sizeY);
    this->_windowSize = glm::vec2(sizeX, sizeY);
    this->_frameBuffers.resize(0);

    this->_textures["Crate.png"] = loadTexture("Resources/Textures/Crate.png");
    this->_textures["Crate_specular.png"] = loadTexture("Resources/Textures/Crate_specular.png");
    this->_textures["Crate_emission.png"] = loadTexture("Resources/Textures/Crate_emission.png");

    this->_models[0] = RenderModel(loadFile2("Resources/Models/Fox.dae"), this->_textures);
    this->_models[1] = RenderModel(loadFile2("Resources/Models/Crate.dae"), this->_textures);
    this->_models[2] = RenderModel(loadFile2("Resources/Models/Cube.dae"), this->_textures);

    this->_skyboxes[0] = loadCubemap({"Resources/Skybox/right.jpg", "Resources/Skybox/left.jpg", "Resources/Skybox/top.jpg", "Resources/Skybox/bottom.jpg",
                                      "Resources/Skybox/front.jpg", "Resources/Skybox/back.jpg"});

    this->_shaders["Camera"] = Shader(loadFile2("Resources/Shaders/camera.vs"), loadFile2("Resources/Shaders/camera.fs"));
    this->_shaders["Picking"] = Shader(loadFile2("Resources/Shaders/camera.vs"), loadFile2("Resources/Shaders/picking.fs"));
    this->_shaders["Outline"] = Shader(loadFile2("Resources/Shaders/outline.vs"), loadFile2("Resources/Shaders/outline.fs"));
    this->_shaders["Skybox"] = Shader(loadFile2("Resources/Shaders/skybox.vs"), loadFile2("Resources/Shaders/skybox.fs"));
    this->_shaders["Billboard"] = Shader(loadFile2("Resources/Shaders/billboard.vs"), loadFile2("Resources/Shaders/billboard.fs"));
    this->_shaders["Grid"] = Shader(loadFile2("Resources/Shaders/grid.vs"), loadFile2("Resources/Shaders/grid.fs"));
    this->_renderView.setShaders(this->_shaders);
}

Graphic::~Graphic()
{
    glfwDestroyWindow(this->_window);
}

void Graphic::setGLFWContext(int sizeX, int sizeY, std::string title)
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");
    this->_window = glfwCreateWindow(sizeX, sizeY, title.c_str(), nullptr, nullptr);
    if (!this->_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(this->_window);
    this->setCallBacks();
    glfwSwapInterval(1); // Enable vsync

    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Failed to initialize GLEW");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void Graphic::setCallBacks()
{
    glfwSetWindowUserPointer(this->_window, this);
    auto keyboardPressCallback = [](GLFWwindow *w, int key, int scan, int action, int mods) {
        static_cast<Graphic *>(glfwGetWindowUserPointer(w))->handleKeyboardPress(key, scan, action, mods);
    };
    glfwSetKeyCallback(this->_window, keyboardPressCallback);

    auto resizeCallback = [](GLFWwindow *w, int width, int height) { static_cast<Graphic *>(glfwGetWindowUserPointer(w))->handleResizeFramebuffer(width, height); };
    glfwSetFramebufferSizeCallback(this->_window, resizeCallback);
}

void Graphic::handleKeyboardPress(int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(this->_window, true);
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        this->_mouseMovement = !this->_mouseMovement;
        glfwSetInputMode(this->_window, GLFW_CURSOR, this->_mouseMovement ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        if (this->_mouseMovement) {
            this->setMousePosition(glm::vec2(this->_gameViewPort.z / 2 + this->_gameViewPort.x, this->_gameViewPort.w / 2 + this->_gameViewPort.y));
            this->setMouseOffset(glm::vec2(this->_gameViewPort.z / 2 + this->_gameViewPort.x, this->_gameViewPort.w / 2 + this->_gameViewPort.y));
        }
        glfwSetCursorPos(this->_window, this->_gameViewPort.z / 2 + this->_gameViewPort.x, this->_gameViewPort.w / 2 + this->_gameViewPort.y);
    }
}

void Graphic::handleResizeFramebuffer(int width, int height)
{
    this->_windowSize = glm::vec2(width, height);
    glViewport(0, 0, this->_windowSize.x, this->_windowSize.y);
    this->_gameViewPort = glm::vec4(0.0f, 0.0f, this->_windowSize.x, this->_windowSize.y);
    this->_sceneViewPort = glm::vec4(0.0f, 0.0f, this->_windowSize.x, this->_windowSize.y);

    for (auto fb : this->_frameBuffers) {
        glBindTexture(GL_TEXTURE_2D, fb.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, fb.depthbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb.depthbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

GLFWwindow *Graphic::getWindow()
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

RenderView &Graphic::getRenderView()
{
    return this->_renderView;
}

std::vector<FrameBuffer> &Graphic::getFrameBuffers()
{
    return this->_frameBuffers;
}

void Graphic::setRunning(bool running)
{
    this->_running = running;
}

bool Graphic::getRunning()
{
    return this->_running;
}

void Graphic::setPaused(bool paused)
{
    this->_paused = paused;
}

bool Graphic::getPaused()
{
    return this->_paused;
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

void Graphic::setGameViewPort(glm::vec4 viewPort)
{
    this->_gameViewPort = viewPort;
}

glm::vec4 Graphic::getGameViewPort()
{
    return this->_gameViewPort;
}

void Graphic::setSceneViewPort(glm::vec4 viewPort)
{
    this->_sceneViewPort = viewPort;
}

glm::vec4 Graphic::getSceneViewPort()
{
    return this->_sceneViewPort;
}

void Graphic::setWindowSize(glm::vec2 windowSize)
{
    this->_windowSize = windowSize;
}

glm::vec2 Graphic::getWindowSize()
{
    return this->_windowSize;
}
