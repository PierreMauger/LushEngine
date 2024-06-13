#include "Script/ScriptClass.hpp"

using namespace Lush;

ScriptClass::ScriptClass(MonoDomain *domain, MonoClass *monoClass, MonoClass *componentClass)
{
    try {
        this->load(domain, monoClass, componentClass);
        this->loadAttributes();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void ScriptClass::load(MonoDomain *domain, MonoClass *monoClass, MonoClass *componentClass)
{
    this->_domain = domain;
    this->_class = monoClass;

    this->_methods["ctor"] = mono_class_get_method_from_name(componentClass, ".ctor", 1);
    this->_methods["onInit"] = mono_class_get_method_from_name(this->_class, "onInit", 0);
    this->_methods["onUpdate"] = mono_class_get_method_from_name(this->_class, "onUpdate", 1);
    this->_methods["onDestroy"] = mono_class_get_method_from_name(this->_class, "onDestroy", 0);

    this->_methods["onCollisionEnter"] = mono_class_get_method_from_name(this->_class, "onCollisionEnter", 1);
    this->_methods["onCollisionStay"] = mono_class_get_method_from_name(this->_class, "onCollisionStay", 1);
    this->_methods["onCollisionExit"] = mono_class_get_method_from_name(this->_class, "onCollisionExit", 1);
}

void ScriptClass::loadAttributes()
{
    int fieldCount = mono_class_num_fields(this->_class);
    void *iter = nullptr;

    MonoObject *instance = mono_object_new(this->_domain, this->_class);
    if (!instance) {
        std::cout << "[Toast Error]Failed to create instance of class: " + std::string(mono_class_get_name(this->_class)) << std::endl;
        return;
    }
    mono_runtime_object_init(instance);
    for (int i = 0; i < fieldCount; i++) {
        MonoClassField *field = mono_class_get_fields(this->_class, &iter);
        const char *fieldName = mono_field_get_name(field);
        uint32_t fieldAttrs = mono_field_get_flags(field);

        MonoType *fieldType = mono_field_get_type(field);
        // const char *fieldTypeName = mono_type_get_name(fieldType); // ex: System.Int32
        MonoClass *fieldClass = mono_class_from_mono_type(fieldType);
        const char *fieldClassName = mono_class_get_name(fieldClass); // ex: Int32

        bool isSerialized = fieldAttrs & MONO_FIELD_ATTR_PUBLIC;
        if (std::string(fieldClassName) == "Single") {
            float defaultValue;
            mono_field_get_value(instance, field, &defaultValue);
            this->_fields[fieldName] = {fieldClassName, field, defaultValue, isSerialized};
        } else if (std::string(fieldClassName) == "Entity" || std::string(fieldClassName) == "UInt64") {
            unsigned long defaultValue;
            mono_field_get_value(instance, field, &defaultValue);
            this->_fields[fieldName] = {fieldClassName, field, defaultValue, isSerialized};
        } else if (std::string(fieldClassName) == "String") {
            MonoString *defaultValue;
            mono_field_get_value(instance, field, &defaultValue);
            this->_fields[fieldName] = {fieldClassName, field, defaultValue ? std::string(mono_string_to_utf8(defaultValue)) : "", isSerialized};
        } else if (std::string(fieldClassName) == "Vector3") {
            float *defaultValue = new float[3]{0.0f, 0.0f, 0.0f};
            mono_field_get_value(instance, field, defaultValue);
            this->_fields[fieldName] = {fieldClassName, field, glm::vec3(defaultValue[0], defaultValue[1], defaultValue[2]), isSerialized};
        } else {
            this->_fields[fieldName] = {fieldClassName, field, isSerialized};
        }
    }
}

void ScriptClass::reload(MonoDomain *domain, MonoClass *scriptClass, MonoClass *componentClass)
{
    this->_methods.clear();
    this->_fields.clear();
    this->load(domain, scriptClass, componentClass);
    this->loadAttributes();
}

MonoMethod *ScriptClass::getMethod(const std::string &name)
{
    if (this->_methods.contains(name))
        return this->_methods[name];
    return nullptr;
}

std::unordered_map<std::string, FieldInfo> &ScriptClass::getFields()
{
    return this->_fields;
}

MonoDomain *ScriptClass::getDomain()
{
    return this->_domain;
}

MonoClass *ScriptClass::getClass()
{
    return this->_class;
}
