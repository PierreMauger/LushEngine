#include "Script/ScriptClass.hpp"

using namespace Lush;

ScriptClass::ScriptClass(MonoDomain *domain, MonoClass *sciptClass, MonoClass *entityClass)
{
    try {
        this->load(domain, sciptClass, entityClass);
        this->loadAttributes();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void ScriptClass::load(MonoDomain *domain, MonoClass *sciptClass, MonoClass *entityClass)
{
    this->_domain = domain;
    this->_class = sciptClass;

    this->_methods["ctor"] = mono_class_get_method_from_name(entityClass, ".ctor", 1);
    this->_methods["onInit"] = mono_class_get_method_from_name(this->_class, "onInit", 0);
    this->_methods["onUpdate"] = mono_class_get_method_from_name(this->_class, "onUpdate", 1);

    for (auto it = this->_methods.begin(); it != this->_methods.end(); it++)
        if (!it->second)
            throw std::runtime_error("mono_class_get_method_from_name failed for " + it->first);
}

void ScriptClass::loadAttributes()
{
    int fieldCount = mono_class_num_fields(this->_class);
    void *iter = NULL;

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

void ScriptClass::reload(MonoDomain *domain, MonoClass *sciptClass, MonoClass *entityClass)
{
    this->_methods.clear();
    this->_fields.clear();
    this->load(domain, sciptClass, entityClass);
    this->loadAttributes();
}

MonoMethod *ScriptClass::getMethod(std::string name)
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
