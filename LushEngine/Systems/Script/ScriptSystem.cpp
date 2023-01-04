#include "Systems/Script/ScriptSystem.hpp"

using namespace Lush;

ScriptSystem::ScriptSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager)
{
    this->_graphic = graphic;
    // TODO Replace all COMPONENT_TYPE_COUNT to use more than one script
    entityManager.addMaskCategory(ComponentType::COMPONENT_TYPE_COUNT);
    this->_scripts.push_back(std::make_unique<ScriptClass>("Spin"));
    this->_graphic->getScriptNames().push_back("Spin");
    ScriptGlue::registerFunctions();
}

ScriptSystem::~ScriptSystem()
{
    this->_instances.clear();
    this->_scripts.clear();
}

void ScriptSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    if (this->buttonChanged()) {
        if (!this->_graphic->getRunning()) {
            this->_instances.clear();
            return;
        }
        for (auto id : entityManager.getMaskCategory(ComponentType::COMPONENT_TYPE_COUNT))
            this->_instances.push_back(std::make_unique<ScriptInstance>(*this->_scripts[0], id));
        for (auto &instance : this->_instances)
            instance->init();
    }
    if (this->_graphic->getPaused() || !this->_graphic->getRunning())
        return;
    for (auto &instance : this->_instances)
        instance->update(glfwGetTime());
}

bool ScriptSystem::buttonChanged()
{
    if (this->_initialized && !this->_graphic->getRunning()) {
        this->_initialized = false;
        return true;
    } else if (!this->_initialized && this->_graphic->getRunning()) {
        this->_initialized = true;
        return true;
    }
    return false;
}
