#include "Script/ScriptClass.hpp"

using namespace Lush;

ScriptClass::ScriptClass(std::string name)
{
    unsetenv("TERM");

    this->_domain = nullptr;
    this->_assembly = nullptr;
    this->_entityAssembly = nullptr;
    this->_image = nullptr;
    this->_entityImage = nullptr;
    this->_class = nullptr;
    this->_entityClass = nullptr;

    try {
        this->loadScript(name);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

ScriptClass::~ScriptClass()
{
    mono_jit_cleanup(this->_domain);
}

void ScriptClass::loadScript(std::string name)
{
    std::string scriptPath = "Resources/Scripts/" + name + ".cs";
    std::string assemblyPath = "Resources/Scripts/" + name + ".dll";

    if (system(std::string("mcs -target:library -out:" + assemblyPath + " " + scriptPath + " -r:Resources/Scripts/Base.dll").c_str()))
        throw std::runtime_error("mcs failed");

    // Initialize the JIT domain
    this->_domain = mono_jit_init("LushJIT");
    if (!this->_domain)
        throw std::runtime_error("mono_jit_init failed");

    // Load the assembly
    this->_assembly = mono_domain_assembly_open(this->_domain, assemblyPath.c_str());
    if (!this->_assembly)
        throw std::runtime_error("mono_domain_assembly_open failed for " + name + ".dll");
    this->_entityAssembly = mono_domain_assembly_open(this->_domain, "Resources/Scripts/Base.dll");
    if (!this->_entityAssembly)
        throw std::runtime_error("mono_domain_assembly_open failed for Base.dll");

    // Create a new image from the assembly
    this->_image = mono_assembly_get_image(this->_assembly);
    if (!this->_image)
        throw std::runtime_error("mono_assembly_get_image failed for " + name + ".dll");
    this->_entityImage = mono_assembly_get_image(this->_entityAssembly);
    if (!this->_entityImage)
        throw std::runtime_error("mono_assembly_get_image failed for Entity");

    // Get the class from the image
    this->_entityClass = mono_class_from_name(this->_entityImage, "", "Entity");
    if (!this->_entityClass)
        throw std::runtime_error("mono_class_from_name failed for Entity");
    this->_class = mono_class_from_name(this->_image, "", name.c_str());
    if (!this->_class)
        throw std::runtime_error("mono_class_from_name failed for " + name);

    this->_methods["ctor"] = mono_class_get_method_from_name(this->_entityClass, ".ctor", 1);
    this->_methods["onInit"] = mono_class_get_method_from_name(this->_class, "onInit", 0);
    this->_methods["onUpdate"] = mono_class_get_method_from_name(this->_class, "onUpdate", 1);

    // Check if all methods were found
    for (auto it = this->_methods.begin(); it != this->_methods.end(); it++)
        if (!it->second)
            throw std::runtime_error("mono_class_get_method_from_name failed for " + it->first);
}

MonoMethod *ScriptClass::getMethod(std::string name)
{
    if (this->_methods.find(name) != this->_methods.end())
        return this->_methods[name];
    return nullptr;
}

MonoDomain *ScriptClass::getDomain()
{
    return this->_domain;
}

MonoClass *ScriptClass::getClass()
{
    return this->_class;
}
