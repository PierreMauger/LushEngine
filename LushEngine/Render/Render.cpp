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

    this->_windowWidth = 800;
    this->_windowHeight = 600;
    this->_window = glfwCreateWindow(this->_windowWidth, this->_windowHeight, "Lush Engine", nullptr, nullptr);

    if (this->_window == NULL)
        std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(this->_window);

    if (glewInit() != GLEW_OK)
        std::runtime_error("GLEW failed to initialize");
    if (!GLEW_VERSION_2_1)
        throw std::runtime_error("GLEW does not support OpenGL 2.1");
    this->_camera = std::make_unique<Camera>(this->_windowWidth, this->_windowHeight);
    this->showImGuiCamera = true;

    glGenFramebuffers(1, &this->_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_framebuffer);
    glGenTextures(1, &this->_texture);
    glBindTexture(GL_TEXTURE_2D, this->_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, this->_windowWidth, this->_windowHeight, 0, GL_RED_INTEGER, GL_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenRenderbuffers(1, &this->_depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->_depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->_windowWidth, this->_windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->_depthbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00); // disable stencil writing
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(this->_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    this->_mouseX = 0;
    this->_mouseY = 0;
    this->_hover = 0;
    std::shared_ptr<Model> model = std::make_shared<Model>("resources/models/Cube.dae");
    this->_objects[0] = std::unique_ptr<RenderObject>(new StaticModel(GameObject(0, "Cube", glm::vec3(0.0f)), model));
    this->_objects[1] = std::unique_ptr<RenderObject>(new StaticModel(GameObject(0, "Cube", glm::vec3(2.0f, 0.0f, 0.5f)), model));
}

Render::~Render()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (this->_window != NULL)
        glfwDestroyWindow(this->_window);
    glfwTerminate();
}

void Render::run()
{
    while (this->_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        this->draw();
        this->handleMouse();
        if (glfwWindowShouldClose(this->_window)) {
            this->_running = false; // because it can sometimes send the quit message twice
            this->sendMessage(Message(Packet(), BaseCommand::QUIT, Module::BROADCAST));
        }
        this->_messageBus->notify(Module::RENDER, this->_functionList);
    }
}

void Render::draw()
{
    this->drawImGui();
    this->drawScene();
    this->drawPicking();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(this->_window);
}

void Render::handleMouse()
{
    glfwPollEvents();
    this->_hover = -1;

    if (ImGui::GetIO().WantCaptureMouse)
        return;
    glfwGetCursorPos(this->_window, &this->_mouseX, &this->_mouseY);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_framebuffer);
    glReadPixels(this->_mouseX, this->_windowHeight - this->_mouseY, 1, 1, GL_RED_INTEGER, GL_INT, &this->_hover);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (glfwGetMouseButton(this->_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        if (this->_hover > 0 && this->_objects.find(this->_hover - 1) != this->_objects.end())
            this->_objects[this->_hover - 1]->setSelected(true);
}

void Render::drawImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (this->showImGuiCamera)
        this->_camera->showImGui(&this->showImGuiCamera);
    for (auto &[key, object] : this->_objects)
        if (object->isSelected())
            object->showImGui(key);

    ImGui::Render();
}

void Render::drawScene()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    this->_camera->getShader().use();
    this->_camera->setShader(0.0f);
    for (auto &[key, object] : this->_objects) {
        object->setHovered(this->_hover - 1 == key);
        object->draw(*this->_camera);
    }
}

void Render::drawPicking()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->_framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->_camera->getPicking().use();
    this->_camera->setPicking();
    for (auto &[key, object] : this->_objects) {
        this->_camera->getPicking().setInt("id", key + 1);
        object->draw(*this->_camera);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
