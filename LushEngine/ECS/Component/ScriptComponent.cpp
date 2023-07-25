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

void ScriptComponent::serialize(boost::archive::binary_oarchive &ar, [[maybe_unused]] const unsigned int version)
{
    auto size = this->_fields.size();
    ar &size;

    for (auto &[name, value] : this->_fields) {
        ar &name;
        ar &std::string(value.type().name());
        if (value.type() == typeid(float))
            ar &std::any_cast<float &>(value);
        else if (value.type() == typeid(unsigned long))
            ar &std::any_cast<unsigned long &>(value);
    }
}

void ScriptComponent::serialize(boost::archive::binary_iarchive &ar, [[maybe_unused]] const unsigned int version)
{
    auto size = this->_fields.size();
    ar &size;

    for (std::size_t i = 0; i < size; i++) {
        std::string name;
        ar &name;
        std::string type;
        ar &type;

        std::any value;
        if (type == typeid(float).name()) {
            float elem{};
            ar &elem;
            value = elem;
        } else if (type == typeid(unsigned long).name()) {
            unsigned long elem{};
            ar &elem;
            value = elem;
        }
        this->_fields[name] = value;
    }
}
