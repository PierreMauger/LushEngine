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
    this->_camera = std::make_unique<Camera>(800.0f, 600.0f);

    glEnable(GL_DEPTH_TEST);
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

    std::shared_ptr<Model> model = std::make_shared<Model>("resources/models/Cube.dae");
    this->_objects[0] = std::unique_ptr<RenderObject>(new StaticModel(GameObject(0, "Cube", glm::vec3(0.0f)), model));
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
        if (glfwWindowShouldClose(this->_window)) {
            this->_running = false; // because it can sometimes send the quit message twice
            this->sendMessage(Message(Packet(), BaseCommand::QUIT, Module::BROADCAST));
        }
        this->_messageBus->notify(Module::RENDER, this->_functionList);
    }
}

void Render::draw()
{
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Another Window");
    ImGui::Text("Hello from another window!");
    ImGui::End();

    ImGui::Render();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->_camera->getShader().use();
    this->_camera->setShader(0.0f);
    for (auto &[key, object] : this->_objects)
        object->draw(*this->_camera);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(this->_window);
}
