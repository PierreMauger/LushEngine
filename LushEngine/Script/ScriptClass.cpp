#include "Script/ScriptClass.hpp"

using namespace Lush;

ScriptClass::ScriptClass(File &file) : Resource(ResourceType::SCRIPT, file)
{
    this->_domain = nullptr;
    this->_assembly = nullptr;
    this->_entityAssembly = nullptr;
    this->_image = nullptr;
    this->_entityImage = nullptr;
    this->_class = nullptr;
    this->_entityClass = nullptr;

    try {
        this->load(file);
        this->loadAttributes();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void ScriptClass::load(File &file)
{
    std::string name = file.getName();
    std::string scriptPath = "Resources/Scripts/" + name + ".cs";
    std::string assemblyPath = "Resources/Scripts/" + name + ".dll";

    if (system(std::string("mcs -target:library -out:" + assemblyPath + " " + scriptPath + " -r:Resources/CoreScripts/Core.dll").c_str()))
        throw std::runtime_error("mcs failed");

    this->_domain = mono_domain_create_appdomain((char *)name.c_str(), nullptr);

    this->_assembly = mono_domain_assembly_open(this->_domain, assemblyPath.c_str());
    if (!this->_assembly)
        throw std::runtime_error("mono_domain_assembly_open failed for " + name + ".dll");
    this->_entityAssembly = mono_domain_assembly_open(this->_domain, "Resources/CoreScripts/Core.dll");
    if (!this->_entityAssembly)
        throw std::runtime_error("mono_domain_assembly_open failed for Core.dll");

    this->_image = mono_assembly_get_image(this->_assembly);
    if (!this->_image)
        throw std::runtime_error("mono_assembly_get_image failed for " + name + ".dll");
    this->_entityImage = mono_assembly_get_image(this->_entityAssembly);
    if (!this->_entityImage)
        throw std::runtime_error("mono_assembly_get_image failed for Entity");

    this->_entityClass = mono_class_from_name(this->_entityImage, "", "Entity");
    if (!this->_entityClass)
        throw std::runtime_error("mono_class_from_name failed for Entity");
    this->_class = mono_class_from_name(this->_image, "", name.c_str());
    if (!this->_class)
        throw std::runtime_error("mono_class_from_name failed for " + name);

    this->_methods["ctor"] = mono_class_get_method_from_name(this->_entityClass, ".ctor", 1);
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

void ScriptClass::reload(File &file)
{
    this->_assembly = nullptr;
    this->_entityAssembly = nullptr;
    this->_image = nullptr;
    this->_entityImage = nullptr;
    this->_class = nullptr;
    this->_entityClass = nullptr;
    this->_methods.clear();

    mono_domain_unload(this->_domain);
    this->load(file);
    this->loadAttributes();
}

MonoMethod *ScriptClass::getMethod(std::string name)
{
    if (this->_methods.find(name) != this->_methods.end())
        return this->_methods[name];
    return nullptr;
}

std::map<std::string, FieldInfo> &ScriptClass::getFields()
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
