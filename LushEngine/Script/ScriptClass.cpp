#include "Script/ScriptClass.hpp"

using namespace Lush;

ScriptClass::ScriptClass(std::string name)
{
    unsetenv("TERM");

    this->_domain = nullptr;
    this->_assembly = nullptr;
    this->_image = nullptr;

    this->loadScript(name);
}

ScriptClass::~ScriptClass()
{
    mono_jit_cleanup(this->_domain);
}

void ScriptClass::loadScript(std::string name)
{
    std::string scriptPath = "Resources/Scripts/" + name + ".cs";
    std::string assemblyPath = "Resources/Scripts/" + name + ".dll";
    std::string command = "mcs " + scriptPath + " Resources/Scripts/Components.cs" + " Resources/Scripts/InternalCalls.cs" + " Resources/Scripts/Entity.cs" + " -target:library";

    if (system(command.c_str())) {
        std::cout << "mcs failed" << std::endl;
        return;
    }

    this->_domain = mono_jit_init("LushJIT");
    if (!this->_domain) {
        std::cout << "mono_jit_init failed" << std::endl;
        return;
    }

    this->_assembly = mono_domain_assembly_open(this->_domain, assemblyPath.c_str());
    if (!this->_assembly) {
        std::cout << "mono_domain_assembly_open failed" << std::endl;
        return;
    }

    this->_image = mono_assembly_get_image(this->_assembly);
    if (!this->_image) {
        std::cout << "mono_assembly_get_image failed" << std::endl;
        return;
    }

    this->_entityClass = mono_class_from_name(this->_image, "", "Entity");
    if (!this->_entityClass) {
        std::cout << "mono_class_from_name failed" << std::endl;
        return;
    }
    this->_class = mono_class_from_name(this->_image, "", name.c_str());
    if (!this->_class) {
        std::cout << "mono_class_from_name failed" << std::endl;
        return;
    }

    this->_methods["ctor"] = mono_class_get_method_from_name(this->_entityClass, ".ctor", 1);
    this->_methods["onInit"] = mono_class_get_method_from_name(this->_class, "onInit", 0);
    this->_methods["onUpdate"] = mono_class_get_method_from_name(this->_class, "onUpdate", 1);
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
