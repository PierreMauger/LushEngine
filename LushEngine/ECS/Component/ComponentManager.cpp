#include "ECS/Component/ComponentManager.hpp"

using namespace Lush;

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

// void ComponentManager::addEntity(std::size_t id)
// {
//     std::any a = std::nullopt;
//     for (auto &[type, component] : this->_componentArray)
//         component.addValue(id, a);
//     for (auto &[name, scriptComponent] : this->_instanceFields)
//         scriptComponent.addValue(id, a);
// }

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

// void ComponentManager::updateComponent(std::size_t id)
// {
//     std::any a = std::nullopt;
//     for (auto &[type, component] : this->_componentArray)
//         component.updateValue(id, a);
// }

std::unordered_map<std::string, std::any> &ComponentManager::getInstanceFields(const std::string &name, std::size_t id)
{
    return std::any_cast<std::unordered_map<std::string, std::any> &>(this->_instanceFields[name].getValues(id).value());
}

SparseArray &ComponentManager::getAllInstanceFields(const std::string &name)
{
    return this->_instanceFields[name];
}

void ComponentManager::bindInstanceFields(const std::string &name)
{
    if (this->_instanceFields.find(name) == this->_instanceFields.end())
        this->_instanceFields[name] = SparseArray();
}

void ComponentManager::addInstanceFields(const std::string &name, std::size_t id, std::any value)
{
    this->_instanceFields[name].addValue(id, value);
}

void ComponentManager::removeInstanceFields(const std::string &name, std::size_t id)
{
    this->_instanceFields[name].removeValue(id);
}

ComponentManager &ComponentManager::operator=(const ComponentManager &other)
{
    this->_componentArray = other._componentArray;
    this->_orderedMap = other._orderedMap;
    this->_instanceFields = other._instanceFields;
    return *this;
}
