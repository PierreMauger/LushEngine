#include "Script/ScriptInstance.hpp"

using namespace Lush;

ScriptInstance::ScriptInstance(ScriptClass &script, std::size_t id) : _class(script)
{
    this->_instance = mono_object_new(script.getDomain(), script.getClass());
    this->_ctor = script.getMethod("ctor");
    this->_onInit = script.getMethod("onInit");
    this->_onUpdate = script.getMethod("onUpdate");
    this->_id = id;

    if (this->_ctor) {
        void *args[1];
        args[0] = &id;
        mono_runtime_invoke(this->_ctor, this->_instance, args, nullptr);
    }
}

ScriptClass &ScriptInstance::getClass()
{
    return this->_class;
}

std::size_t ScriptInstance::getId()
{
    return this->_id;
}

bool ScriptInstance::getFieldValueInternal(std::string name, void *value)
{
    if (this->_class.getFields().find(name) == this->_class.getFields().end())
        return false;

    FieldInfo field = this->_class.getFields()[name];
    mono_field_get_value(this->_instance, field.field, value);
    return true;
}

void ScriptInstance::setFieldValueInternal(std::string name, void *value)
{
    FieldInfo field = this->_class.getFields()[name];
    mono_field_set_value(this->_instance, field.field, value);
}

void ScriptInstance::init()
{
    if (this->_onInit) {
        void *args[0];
        mono_runtime_invoke(this->_onInit, this->_instance, args, nullptr);
    }
}

void ScriptInstance::update(float time)
{
    if (this->_onUpdate) {
        void *args[1];
        args[0] = &time;
        mono_runtime_invoke(this->_onUpdate, this->_instance, args, nullptr);
    }
}
