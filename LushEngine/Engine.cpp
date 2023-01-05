#include "Engine.hpp"

#define REGEX_VEC3 "\\(([+-]?[0-9]*[.]?[0-9]*)\\s*([+-]?[0-9]*[.]?[0-9]*)\\s*([+-]?[0-9]*[.]?[0-9]*)\\)"

using namespace Lush;

// TODO loader
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
    this->_graphic = std::make_shared<Graphic>(1280, 720, "Lush Engine");

    this->_systemManager.bindSystem(std::make_unique<ScriptSystem>(this->_graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<ControlSystem>(this->_graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<CameraSystem>(this->_graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<RenderSystem>(this->_graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<PickingSystem>(this->_graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<SceneSystem>(this->_graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<GUISystem>(this->_graphic));

    this->_componentManager.bindComponent<Transform>();
    this->_componentManager.bindComponent<Velocity>();
    this->_componentManager.bindComponent<Model>();
    this->_componentManager.bindComponent<Camera>();
    this->_componentManager.bindComponent<Light>();
    this->_componentManager.bindComponent<Control>();
    this->_componentManager.bindComponent<CubeMap>();
    this->_componentManager.bindComponent<BillBoard>();

    this->loadScene();
}

void Engine::loadScene()
{
    std::string scene = loadFile("Resources/Scenes/scene");

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
                        this->_componentManager.addComponent<Model>(id);
                        break;
                    case 3:
                        this->_componentManager.addComponent<Camera>(id);
                        break;
                    case 4:
                        this->_componentManager.addComponent<Light>(id);
                        break;
                    case 5:
                        this->_componentManager.addComponent<Control>(id);
                        break;
                    case 6:
                        this->_componentManager.addComponent<CubeMap>(id);
                        break;
                    case 7:
                        this->_componentManager.addComponent<BillBoard>(id);
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

void Engine::run()
{
    std::streambuf *oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(this->_graphic->getStringStream().rdbuf());

    while (!glfwWindowShouldClose(this->_graphic->getWindow())) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        this->updateMouse();

        this->_systemManager.updateSystems(this->_entityManager, this->_componentManager);

        glfwSwapBuffers(this->_graphic->getWindow());
        glfwPollEvents();
        this->_graphic->setLastTime(glfwGetTime());
    }
    std::cout.rdbuf(oldCoutStreamBuf);
}

void Engine::updateMouse()
{
    double x, y;
    glfwGetCursorPos(this->_graphic->getWindow(), &x, &y);
    if (this->_graphic->getMouseMovement())
        this->_graphic->setMouseOffset(glm::vec2(x, y));
    else
        this->_graphic->setMousePosition(glm::vec2(x, y));
}

EntityManager &Engine::getEntityManager()
{
    return this->_entityManager;
}

ComponentManager &Engine::getComponentManager()
{
    return this->_componentManager;
}
