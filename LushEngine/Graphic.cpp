#include "Graphic.hpp"

using namespace Lush;

Graphic::Graphic(int sizeX, int sizeY, std::string title) : _renderView(sizeX / sizeY)
{
    this->setGLFWContext(sizeX, sizeY, title);

    this->_mousePosition = glm::vec2(sizeX / 2, sizeY / 2);
    this->_gameViewPort = glm::vec4(0.0f, 0.0f, sizeX, sizeY);
    this->_sceneViewPort = glm::vec4(0.0f, 0.0f, sizeX, sizeY);
    this->_windowSize = glm::vec2(sizeX, sizeY);

    this->_files["Fox"] = File("Resources/Models/Fox.dae");
    this->_files["Crate"] = File("Resources/Models/Crate.dae");
    this->_files["Cube"] = File("Resources/Models/Cube.dae");
    this->_files["Maxwell"] = File("Resources/Models/Maxwell.dae");

    this->_files["model.vs"] = File("Resources/Shaders/model.vs");
    this->_files["model.fs"] = File("Resources/Shaders/model.fs");
    this->_files["picking.fs"] = File("Resources/Shaders/picking.fs");
    this->_files["billboard.vs"] = File("Resources/Shaders/billboard.vs");
    this->_files["billboard.fs"] = File("Resources/Shaders/billboard.fs");
    this->_files["outline.vs"] = File("Resources/Shaders/outline.vs");
    this->_files["outline.fs"] = File("Resources/Shaders/outline.fs");
    this->_files["skybox.vs"] = File("Resources/Shaders/skybox.vs");
    this->_files["skybox.fs"] = File("Resources/Shaders/skybox.fs");
    this->_files["grid.vs"] = File("Resources/Shaders/grid.vs");
    this->_files["grid.fs"] = File("Resources/Shaders/grid.fs");
    this->_files["map.vs"] = File("Resources/Shaders/map.vs");
    this->_files["map.fs"] = File("Resources/Shaders/map.fs");
    this->_files["map.tcs"] = File("Resources/Shaders/map.tcs");
    this->_files["map.tes"] = File("Resources/Shaders/map.tes");

    this->_files["Spin"] = File("Resources/Scripts/Spin.cs");
    this->_files["MaxwellScript"] = File("Resources/Scripts/Maxwell.cs");
    this->_files["Controlable"] = File("Resources/Scripts/Controlable.cs");

    this->_files["Crate.png"] = File("Resources/Textures/Crate.png");
    this->_files["Crate_specular.png"] = File("Resources/Textures/Crate_specular.png");
    this->_files["Crate_emission.png"] = File("Resources/Textures/Crate_emission.png");
    this->_files["Maxwell.jpeg"] = File("Resources/Textures/Maxwell.jpeg");
    this->_files["Whiskers.png"] = File("Resources/Textures/Whiskers.png");
    this->_files["heightMap.png"] = File("Resources/Textures/heightMap.png");
    this->_files["Audio.png"] = File("Resources/Textures/Editor/Audio.png");
    this->_files["Camera.png"] = File("Resources/Textures/Editor/Camera.png");
    this->_files["DirectionalLight.png"] = File("Resources/Textures/Editor/DirectionalLight.png");
    this->_files["Geometry.png"] = File("Resources/Textures/Editor/Geometry.png");
    this->_files["PointLight.png"] = File("Resources/Textures/Editor/PointLight.png");
    this->_files["SpotLight.png"] = File("Resources/Textures/Editor/SpotLight.png");

    this->_files["scene.xml"] = File("Resources/Scenes/scene.xml");

    this->_scene = std::make_shared<Scene>(this->_files["scene.xml"]);

    this->_textures["Crate.png"] = Texture(this->_files["Crate.png"]);
    this->_textures["Crate_specular.png"] = Texture(this->_files["Crate_specular.png"]);
    this->_textures["Crate_emission.png"] = Texture(this->_files["Crate_emission.png"]);
    this->_textures["Maxwell.jpeg"] = Texture(this->_files["Maxwell.jpeg"]);
    this->_textures["Whiskers.png"] = Texture(this->_files["Whiskers.png"]);
    this->_textures["heightMap.png"] = Texture(this->_files["heightMap.png"]);
    this->_textures["Audio.png"] = Texture(this->_files["Audio.png"]);
    this->_textures["Camera.png"] = Texture(this->_files["Camera.png"]);
    this->_textures["DirectionalLight.png"] = Texture(this->_files["DirectionalLight.png"]);
    this->_textures["Geometry.png"] = Texture(this->_files["Geometry.png"]);
    this->_textures["PointLight.png"] = Texture(this->_files["PointLight.png"]);
    this->_textures["SpotLight.png"] = Texture(this->_files["SpotLight.png"]);

    this->_models["Fox"] = RenderModel(this->_files["Fox"], this->_textures);
    this->_models["Crate"] = RenderModel(this->_files["Crate"], this->_textures);
    this->_models["Cube"] = RenderModel(this->_files["Cube"], this->_textures);
    this->_models["Maxwell"] = RenderModel(this->_files["Maxwell"], this->_textures);

    this->_files["right.jpg"] = File("Resources/Skybox/right.jpg");
    this->_files["left.jpg"] = File("Resources/Skybox/left.jpg");
    this->_files["top.jpg"] = File("Resources/Skybox/top.jpg");
    this->_files["bottom.jpg"] = File("Resources/Skybox/bottom.jpg");
    this->_files["front.jpg"] = File("Resources/Skybox/front.jpg");
    this->_files["back.jpg"] = File("Resources/Skybox/back.jpg");

    std::vector<File> files = {this->_files["right.jpg"], this->_files["left.jpg"], this->_files["top.jpg"], this->_files["bottom.jpg"],
                               this->_files["front.jpg"], this->_files["back.jpg"]};
    this->_skyboxes["Sky"] = CubeMap(files);

    this->_shaders["Model"] = Shader(this->_files["model.vs"], this->_files["model.fs"]);
    this->_shaders["PickingModel"] = Shader(this->_files["model.vs"], this->_files["picking.fs"]);
    this->_shaders["PickingBillboard"] = Shader(this->_files["billboard.vs"], this->_files["picking.fs"]);
    this->_shaders["Outline"] = Shader(this->_files["outline.vs"], this->_files["outline.fs"]);
    this->_shaders["Skybox"] = Shader(this->_files["skybox.vs"], this->_files["skybox.fs"]);
    this->_shaders["Billboard"] = Shader(this->_files["billboard.vs"], this->_files["billboard.fs"]);
    this->_shaders["Grid"] = Shader(this->_files["grid.vs"], this->_files["grid.fs"]);
    this->_shaders["Map"] = Shader(this->_files["map.vs"], this->_files["map.fs"], File(), this->_files["map.tcs"], this->_files["map.tes"]);
    this->_renderView.setShaders(this->_shaders);

    this->_map = std::make_unique<MapMesh>(2624, 1756);

    this->_cursors[0] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    this->_cursors[1] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    this->_cursors[2] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

    // GLFWimage images[1];
    // images[0].pixels = stbi_load("Resources/Icon/Lush.png", &images[0].width, &images[0].height, 0, 4);
    // this->_cursors[...] = glfwCreateCursor(&images[0], 0, 0);
    // stbi_image_free(images[0].pixels);
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
    // this->_gameViewPort = glm::vec4(0.0f, 0.0f, this->_windowSize.x, this->_windowSize.y);
    // this->_sceneViewPort = glm::vec4(0.0f, 0.0f, this->_windowSize.x, this->_windowSize.y);

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

std::map<std::string, Shader> &Graphic::getShaders()
{
    return this->_shaders;
}

std::map<std::string, Texture> &Graphic::getTextures()
{
    return this->_textures;
}

std::map<std::string, RenderModel> &Graphic::getModels()
{
    return this->_models;
}

std::map<std::string, CubeMap> &Graphic::getSkyboxes()
{
    return this->_skyboxes;
}

MapMesh &Graphic::getMap()
{
    return *this->_map;
}

RenderView &Graphic::getRenderView()
{
    return this->_renderView;
}

std::map<std::string, FrameBuffer> &Graphic::getFrameBuffers()
{
    return this->_frameBuffers;
}

std::map<std::string, File> &Graphic::getFiles()
{
    return this->_files;
}

FrameBuffer &Graphic::getFrameBuffer(std::string name)
{
    return this->_frameBuffers[name];
}

std::map<std::string, ScriptClass> &Graphic::getScripts()
{
    return this->_scripts;
}

std::vector<ScriptInstance> &Graphic::getInstances()
{
    return this->_instances;
}

std::ostringstream &Graphic::getStringStream()
{
    return this->_stringStream;
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

std::shared_ptr<Scene> Graphic::getScene()
{
    return this->_scene;
}
