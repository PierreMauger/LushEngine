#include "Graphic.hpp"

using namespace Lush;

Graphic::Graphic(int sizeX, int sizeY, std::string title) : _renderView(sizeX / sizeY)
{
    this->setGLFWContext(sizeX, sizeY, title);

    this->_mousePosition = glm::vec2(sizeX / 2, sizeY / 2);
    this->_gameViewPort = glm::vec4(0.0f, 0.0f, sizeX, sizeY);
    this->_sceneViewPort = glm::vec4(0.0f, 0.0f, sizeX, sizeY);
    this->_windowSize = glm::vec2(sizeX, sizeY);

    this->_cursors[0] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    this->_cursors[1] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    this->_cursors[2] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
}

Graphic::~Graphic()
{
    for (std::size_t i = 0; i < this->_cursors.size(); i++)
        glfwDestroyCursor(this->_cursors[i]);
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

    auto mousePressCallback = [](GLFWwindow *w, int button, int action, int mods) { static_cast<Graphic *>(glfwGetWindowUserPointer(w))->handleMousePress(button, action, mods); };
    glfwSetMouseButtonCallback(this->_window, mousePressCallback);
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

    for (auto [name, fb] : this->_frameBuffers) {
        glBindTexture(GL_TEXTURE_2D, fb.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, fb.depthbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb.depthbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}

void Graphic::handleMousePress(int button, int action, [[maybe_unused]] int mods)
{
    if (this->_sceneMovement && action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            this->_selectedEntity = this->_hoveredEntity;
        this->_mouseButton = button;
        glfwSetCursor(this->_window, this->_cursors[button]);
    }
    if (action == GLFW_RELEASE) {
        this->_mouseButton = -1;
        glfwSetCursor(this->_window, nullptr);
    }
}

GLFWwindow *Graphic::getWindow()
{
    return this->_window;
}

RenderView &Graphic::getRenderView()
{
    return this->_renderView;
}

std::unordered_map<std::string, FrameBuffer> &Graphic::getFrameBuffers()
{
    return this->_frameBuffers;
}

FrameBuffer &Graphic::getFrameBuffer(std::string name)
{
    return this->_frameBuffers[name];
}

std::ostringstream &Graphic::getStringStream()
{
    return this->_stringStream;
}

void Graphic::setWireframe(bool drawWireframe)
{
    this->_drawWireframe = drawWireframe;
}

bool Graphic::isWireframe()
{
    return this->_drawWireframe;
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

void Graphic::setHoveredEntity(std::size_t hoveredEntity)
{
    this->_hoveredEntity = hoveredEntity;
}

std::size_t Graphic::getHoveredEntity()
{
    return this->_hoveredEntity;
}

void Graphic::setSelectedEntity(std::size_t selectedEntity)
{
    this->_selectedEntity = selectedEntity;
}

std::size_t Graphic::getSelectedEntity()
{
    return this->_selectedEntity;
}

void Graphic::setMouseMovement(bool mouseMovement)
{
    this->_mouseMovement = mouseMovement;
}

bool Graphic::getMouseMovement()
{
    return this->_mouseMovement;
}

void Graphic::setSceneMovement(bool sceneMovement)
{
    this->_sceneMovement = sceneMovement;
}

bool Graphic::getSceneMovement()
{
    return this->_sceneMovement;
}

void Graphic::setMouseButton(int button)
{
    this->_mouseButton = button;
}

int Graphic::getMouseButton()
{
    return this->_mouseButton;
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
