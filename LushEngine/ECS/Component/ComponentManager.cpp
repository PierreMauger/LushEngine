#include "ECS/Component/ComponentManager.hpp"

using namespace Lush;

ComponentManager::ComponentManager()
{
}

std::map<std::type_index, Component> &ComponentManager::getComponentArray()
{
    return this->_componentArray;
}

Component &ComponentManager::getComponent(std::type_index type)
{
    if (this->_componentArray.find(type) == this->_componentArray.end())
        throw std::runtime_error("Component not found");
    return this->_componentArray[type];
}

Component &ComponentManager::getComponent(std::size_t index)
{
    if (index >= this->_componentArray.size())
        throw std::runtime_error("Component not found");
    return this->_componentArray[this->_orderedMap.at(index)];
}

std::type_index ComponentManager::getComponentType(std::size_t index)
{
    if (index >= this->_componentArray.size())
        throw std::runtime_error("Component not found");
    return this->_orderedMap.at(index);
}

void ComponentManager::addEntity(std::size_t id)
{
    for (auto &[type, component] : this->_componentArray)
        component.addValue(id, std::nullopt);
}

void ComponentManager::removeSingleComponent(std::size_t id, std::type_index type)
{
    if (this->_componentArray.find(type) == this->_componentArray.end())
        throw std::runtime_error("Component not found");
    this->_componentArray[type].removeValue(id);
}

void ComponentManager::removeAllComponents(std::size_t id)
{
    for (auto &[type, component] : this->_componentArray)
        component.removeValue(id);
}

void ComponentManager::updateComponent(std::size_t id)
{
    for (auto &[type, component] : this->_componentArray)
        component.updateValue(id, std::nullopt);
}
