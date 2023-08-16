#include "Script/ScriptClass.hpp"

using namespace Lush;

ScriptClass::ScriptClass(MonoDomain *domain, MonoClass *scriptClass, MonoClass *entityClass)
{
    try {
        this->load(domain, scriptClass, entityClass);
        this->loadAttributes();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void ScriptClass::load(MonoDomain *domain, MonoClass *scriptClass, MonoClass *entityClass)
{
    this->_domain = domain;
    this->_class = scriptClass;

    this->_methods["ctor"] = mono_class_get_method_from_name(entityClass, ".ctor", 1);
    this->_methods["onInit"] = mono_class_get_method_from_name(this->_class, "onInit", 0);
    this->_methods["onUpdate"] = mono_class_get_method_from_name(this->_class, "onUpdate", 1);

    this->_methods["onCollisionEnter"] = mono_class_get_method_from_name(this->_class, "onCollisionEnter", 1);
    this->_methods["onCollisionStay"] = mono_class_get_method_from_name(this->_class, "onCollisionStay", 1);
    this->_methods["onCollisionExit"] = mono_class_get_method_from_name(this->_class, "onCollisionExit", 1);
}

void ScriptClass::loadAttributes()
{
    int fieldCount = mono_class_num_fields(this->_class);
    void *iter = nullptr;

    for (int i = 0; i < fieldCount; i++) {
        MonoClassField *field = mono_class_get_fields(this->_class, &iter);
        const char *fieldName = mono_field_get_name(field);
        uint32_t fieldAttrs = mono_field_get_flags(field);

        MonoType *fieldType = mono_field_get_type(field);
        // const char *fieldTypeName = mono_type_get_name(fieldType); // ex: System.Int32
        MonoClass *fieldClass = mono_class_from_mono_type(fieldType);
        const char *fieldClassName = mono_class_get_name(fieldClass); // ex: Int32

        if (fieldAttrs & MONO_FIELD_ATTR_PUBLIC)
            this->_fields[fieldName] = {fieldClassName, field};
    }
}

void ScriptClass::reload(MonoDomain *domain, MonoClass *scriptClass, MonoClass *entityClass)
{
    this->_methods.clear();
    this->_fields.clear();
    this->load(domain, scriptClass, entityClass);
    this->loadAttributes();
}

MonoMethod *ScriptClass::getMethod(const std::string &name)
{
    if (this->_methods.find(name) != this->_methods.end())
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
