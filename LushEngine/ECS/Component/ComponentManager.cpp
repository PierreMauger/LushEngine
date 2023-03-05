#include "ECS/Component/ComponentManager.hpp"

using namespace Lush;

ComponentManager::ComponentManager()
{
}

std::map<std::type_index, SparseArray> &ComponentManager::getComponentArray()
{
    return this->_componentArray;
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
    for (auto &[name, scriptComponent] : this->_instanceFields)
        scriptComponent.addValue(id, std::nullopt);
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

std::unordered_map<std::string, std::any> &ComponentManager::getInstanceFields(std::string name, std::size_t id)
{
    return std::any_cast<std::unordered_map<std::string, std::any> &>(this->_instanceFields[name].getValues(id).value());
}

SparseArray &ComponentManager::getAllInstanceFields(std::string name)
{
    return this->_instanceFields[name];
}

void ComponentManager::bindInstanceFields(std::string name)
{
    this->_instanceFields.try_emplace(name, SparseArray());
}

void ComponentManager::addInstanceFields(std::string name, std::size_t id, std::any value)
{
    this->_instanceFields[name].addValue(id, value);
}

void ComponentManager::removeInstanceFields(std::string name, std::size_t id)
{
    this->_instanceFields[name].removeValue(id);
}
