#include "Systems/Render/RenderSystem.hpp"

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

RenderSystem::RenderSystem(std::shared_ptr<GLFWwindow> window) : _camera(1280, 720)
{
    this->_window = window;

    this->_models[0] = std::make_shared<Model>(loadFile("resources/models/Cube.dae"), std::map<std::string, unsigned int>());
    this->_shaders["Camera"] = std::make_shared<Shader>(loadFile("resources/shaders/camera.vs"), loadFile("resources/shaders/camera.fs"));

    this->_camera.setShaders(this->_shaders);
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
    auto &masks = entityManager.getMasks();
    std::size_t renderable = (InfoComp::TRANSFORM | InfoComp::MODELID);

    this->_camera.use("Camera");
    this->_camera.update();
    this->_camera.setView(0.0f);
    this->_camera.setDirLight(glm::vec3(45.0f, 90.0f, 0.0f));
    this->_camera.setPointLights(std::vector<glm::vec3>());

    for (std::size_t i = 0; i < masks.size(); i++) {
        if (masks[i].has_value() && (masks[i].value() & renderable) == renderable) {
            Transform transform = componentManager.getSingleComponent<Transform>(i);
            ModelID modelID = componentManager.getSingleComponent<ModelID>(i);

            this->_camera.setOnModel(transform.position, transform.scale, transform.rotation);
            if (this->_models.find(modelID.id) != this->_models.end())
                this->_models[modelID.id]->draw(*this->_camera.getShader());
        }
    }
}
