#include "Engine.hpp"
#define REGEX_VEC3 "\\(([+-]?[0-9]*[.]?[0-9]*)\\s*([+-]?[0-9]*[.]?[0-9]*)\\s*([+-]?[0-9]*[.]?[0-9]*)\\)"

using namespace Lush;

std::string loadFile(std::string fileName)
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

Engine::Engine()
{
    this->_entityManager = EntityManager();
    this->_componentManager = ComponentManager();
    this->_systemManager = SystemManager();

    this->initWindow();

    this->_systemManager.bindSystem(std::make_shared<RenderSystem>(this->_window));
    this->_systemManager.bindSystem(std::make_shared<GUISystem>(this->_window));
    this->_systemManager.bindSystem(std::make_shared<ControlSystem>(this->_window));

    this->_componentManager.bindComponent<Transform>();
    this->_componentManager.bindComponent<Velocity>();
    this->_componentManager.bindComponent<ModelID>();
    this->_componentManager.bindComponent<Camera>();
    this->_componentManager.bindComponent<Light>();
    this->_componentManager.bindComponent<Controllable>();

    this->loadScene();
}

void Engine::loadScene()
{
    std::string scene = loadFile("Resources/scene");

    std::vector<std::string> lines;
    std::stringstream ss(scene);
    std::string line;

    while (std::getline(ss, line, '\n'))
        if (line.length() > 1 && line[0] != '#')
            lines.push_back(line);

    for (auto &line : lines) {
        std::regex regex("(\\d+)\\s+(\\d+)\\s+");
        std::smatch match;
        while (std::regex_search(line, match, regex)) {
            std::size_t id = std::stoul(match[1].str());
            std::size_t mask = std::stoul(match[2].str(), nullptr, 2);
            this->_entityManager.addMask(id, mask);
            for (std::size_t i = 0; i < this->_componentManager.getComponentArray().size(); i++) {
                if (mask & (1 << i)) {
                    switch (i) {
                    case 0: {
                        std::regex regex2(std::string(REGEX_VEC3) + "\\s+" + REGEX_VEC3 + "\\s+" + REGEX_VEC3);
                        std::smatch match2;
                        std::regex_search(line, match2, regex2);

                        Transform temp;
                        temp.position = glm::vec3(std::stof(match2[1].str()), std::stof(match2[2].str()), std::stof(match2[3].str()));
                        temp.rotation = glm::vec3(std::stof(match2[4].str()), std::stof(match2[5].str()), std::stof(match2[6].str()));
                        temp.scale = glm::vec3(std::stof(match2[7].str()), std::stof(match2[8].str()), std::stof(match2[9].str()));

                        this->_componentManager.addComponent<Transform>(id, temp);
                        break;
                    }
                    case 1:
                        this->_componentManager.addComponent<Velocity>(id);
                        break;
                    case 2:
                        this->_componentManager.addComponent<ModelID>(id);
                        break;
                    case 3:
                        this->_componentManager.addComponent<Camera>(id);
                        break;
                    case 4:
                        this->_componentManager.addComponent<Light>(id);
                        break;
                    case 5:
                        this->_componentManager.addComponent<Controllable>(id);
                        break;
                    default:
                        break;
                    }
                }
            }
            line = match.suffix().str();
        }
    }
}

void Engine::initWindow()
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

void Engine::run()
{
    while (!glfwWindowShouldClose(this->_window.get())) {
        this->clear();
        this->_systemManager.updateSystems(this->_componentManager, this->_entityManager);
        glfwSwapBuffers(this->_window.get());
        this->draw();
    }
}

void Engine::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Engine::draw()
{
    glfwPollEvents();
}
