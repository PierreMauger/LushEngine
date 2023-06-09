#include "ECS/Component/ScriptComponent.hpp"

using namespace Lush;

std::unordered_map<std::string, std::any> &ScriptComponent::getFields()
{
    return this->_fields;
}

void ScriptComponent::addField(const std::string &name, std::any value)
{
    this->_fields[name] = value;
}

void ScriptComponent::removeField(const std::string &name)
{
    this->_fields.erase(name);
}

bool ScriptComponent::hasField(const std::string &name)
{
    return this->_fields.find(name) != this->_fields.end();
}
