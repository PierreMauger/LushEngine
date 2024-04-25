#include "ECS/Entity/Entity.hpp"

using namespace Lush;

Entity::Entity()
{
}

const UUID &Entity::getUUID() const
{
    return this->_uuid;
}

const std::string &Entity::getName() const
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
    if (it != this->_scriptComponents.end())
        return it->second;
    throw std::runtime_error("Script component not found");
}

void Entity::addScriptComponent(const std::string &className, ScriptComponent &scriptComponent)
{
    this->_scriptComponents[className] = scriptComponent;
}

void Entity::removeScriptComponent(const std::string &className)
{
    this->_scriptComponents.erase(className);
}

bool Entity::hasScriptComponent(const std::string &className)
{
    return this->_scriptComponents.contains(className);
}

std::unordered_map<std::string, std::size_t> &Entity::getScriptIndexes()
{
    return this->_scriptIndexes;
}

void Entity::addScriptIndex(const std::string &className, std::size_t index)
{
    this->_scriptIndexes[className] = index;
}

void Entity::clearScriptIndexes()
{
    this->_scriptIndexes.clear();
}

void Entity::addChild(std::size_t index)
{
    if (std::find(this->_children.begin(), this->_children.end(), index) != this->_children.end())
        return;
    this->_children.push_back(index);
}

void Entity::removeChild(std::size_t index)
{
    this->_children.erase(std::remove(this->_children.begin(), this->_children.end(), index), this->_children.end());
}

std::vector<std::size_t> &Entity::getChildren()
{
    return this->_children;
}

void Entity::setParent(std::size_t index)
{
    this->_parent = index;
}

void Entity::removeParent()
{
    this->_parent.reset();
}

std::optional<std::size_t> Entity::getParent() const
{
    return this->_parent;
}

bool Entity::operator==(const Entity &other) const
{
    return this->_uuid == other._uuid;
}

Entity &Entity::clone(const Entity &other)
{
    this->_uuid = other._uuid;
    this->_name = other._name;
    for (auto &[name, component] : other._components)
        this->_components[name] = component->clone();
    this->_scriptComponents = other._scriptComponents;
    this->_children = other._children;
    this->_parent = other._parent;
    return *this;
}
