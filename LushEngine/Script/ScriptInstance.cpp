#include "Script/ScriptInstance.hpp"

using namespace Lush;

ScriptInstance::ScriptInstance(ScriptClass &script, std::size_t id, std::unordered_map<std::string, std::any> &defaultFields) : _class(script)
{
    this->_instance = mono_object_new(script.getDomain(), script.getClass());
    this->_ctor = script.getMethod("ctor");
    this->_onInit = script.getMethod("onInit");
    this->_onUpdate = script.getMethod("onUpdate");
    this->_onCollisionEnter = script.getMethod("onCollisionEnter");
    this->_onCollisionStay = script.getMethod("onCollisionStay");
    this->_onCollisionExit = script.getMethod("onCollisionExit");
    this->_id = id;

    if (this->_ctor) {
        void *args[1];
        args[0] = &this->_id;
        mono_runtime_invoke(this->_ctor, this->_instance, args, nullptr);
    }
    this->_defaultFields = defaultFields;
}

MonoObject *ScriptInstance::getInstance()
{
    return this->_instance;
}

std::size_t ScriptInstance::getId() const
{
    return this->_id;
}

bool ScriptInstance::getFieldValueInternal(const std::string &name, void *value)
{
    if (this->_class.getFields().find(name) == this->_class.getFields().end())
        return false;

    FieldInfo field = this->_class.getFields()[name];
    if (field.type == "Entity") {
        MonoType *type = mono_field_get_type(field.field);
        MonoClass *klass = mono_class_from_mono_type(type);
        MonoObject *obj;
        mono_field_get_value(this->_instance, field.field, &obj);
        MonoClassField *idField = mono_class_get_field_from_name(klass, "id");
        auto *idPtr = (unsigned long *)mono_object_unbox(mono_field_get_value_object(mono_domain_get(), idField, obj));
        *((unsigned long *)value) = *idPtr;
        return true;
    }
    mono_field_get_value(this->_instance, field.field, value);
    return true;
}

void ScriptInstance::setFieldValueInternal(const std::string &name, void *value)
{
    FieldInfo field = this->_class.getFields()[name];
    if (field.type == "Entity") {
        MonoType *type = mono_field_get_type(field.field);
        MonoClass *klass = mono_class_from_mono_type(type);
        MonoObject *obj = mono_object_new(mono_domain_get(), klass);
        MonoMethod *ctor = mono_class_get_method_from_name(klass, ".ctor", 1);
        void *args[1];
        args[0] = value;
        mono_runtime_invoke(ctor, obj, args, nullptr);
        mono_field_set_value(this->_instance, field.field, obj);
        return;
    }
    mono_field_set_value(this->_instance, field.field, value);
}

void ScriptInstance::init()
{
    for (auto &[name, value] : this->_defaultFields)
        if (this->_class.getFields()[name].type == "Entity")
            this->setFieldValue<unsigned long>(name, std::any_cast<unsigned long>(value));
    if (this->_onInit) {
        void *args[0];
        mono_runtime_invoke(this->_onInit, this->_instance, args, nullptr);
    }
    for (auto &[name, value] : this->_defaultFields) {
        std::string type = this->_class.getFields()[name].type;
        if (type == "Single")
            this->setFieldValue<float>(name, std::any_cast<float>(value));
        else if (type == "UInt64")
            this->setFieldValue<unsigned long>(name, std::any_cast<unsigned long>(value));
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

void ScriptInstance::onCollisionEnter(std::size_t id)
{
    if (this->_onCollisionEnter) {
        void *args[1];
        args[0] = &id;
        mono_runtime_invoke(this->_onCollisionEnter, this->_instance, args, nullptr);
    }
}

void ScriptInstance::onCollisionStay(std::size_t id)
{
    if (this->_onCollisionStay) {
        void *args[1];
        args[0] = &id;
        mono_runtime_invoke(this->_onCollisionStay, this->_instance, args, nullptr);
    }
}

void ScriptInstance::onCollisionExit(std::size_t id)
{
    if (this->_onCollisionExit) {
        void *args[1];
        args[0] = &id;
        mono_runtime_invoke(this->_onCollisionExit, this->_instance, args, nullptr);
    }
}
