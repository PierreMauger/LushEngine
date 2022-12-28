#include "Systems/Script/ScriptSystem.hpp"

using namespace Lush;

ScriptSystem::ScriptSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager)
{
    this->_graphic = graphic;
    this->_scripts.push_back(std::make_shared<Script>("Dog"));
    ScriptGlue::registerFunctions();

    for (auto &script : this->_scripts)
        script->init();
}

ScriptSystem::~ScriptSystem()
{
}

void ScriptSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    for (auto &script : this->_scripts)
        script->update(glfwGetTime());
}
