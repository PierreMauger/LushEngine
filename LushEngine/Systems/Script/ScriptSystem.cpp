#include "Systems/Script/ScriptSystem.hpp"

using namespace Lush;

ScriptSystem::ScriptSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager)
{
    this->_graphic = graphic;
    this->_scripts.push_back(std::make_shared<ScriptClass>("Spin"));
    ScriptGlue::registerFunctions();
    this->_instances.push_back(std::make_shared<ScriptInstance>(*this->_scripts[0], 0));
    this->_instances.push_back(std::make_shared<ScriptInstance>(*this->_scripts[0], 3));

    for (auto &instance : this->_instances)
        instance->init();
}

ScriptSystem::~ScriptSystem()
{
}

void ScriptSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    for (auto &instance : this->_instances)
        instance->update(glfwGetTime());
}
