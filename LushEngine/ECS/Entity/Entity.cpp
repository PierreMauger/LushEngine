#include "ECS/Entity/Entity.hpp"

using namespace Lush;

Entity::Entity()
{
}

std::string Entity::getName() const
{
    return this->_name;
}

void Entity::setName(const std::string &name)
{
    this->_name = name;
}

ScriptComponent &Entity::getScriptComponent(const std::string &className)
{
    auto it = this->_scriptComponents.find(className);
    if (it != this->_scriptComponents.end()) {
        return it->second;
    }
    throw std::runtime_error("Script component not found");
}

void Entity::addScriptComponent(const std::string &className, ScriptComponent scriptComponent)
{
    this->_scriptComponents[className] = scriptComponent;
}

void Entity::removeScriptComponent(const std::string &className)
{
    this->_scriptComponents.erase(className);
}

bool Entity::hasScriptComponent(const std::string &className)
{
    return this->_scriptComponents.find(className) != this->_scriptComponents.end();
}

bool Entity::operator==(const Entity &other) const
{
    return this->_uuid == other._uuid;
}