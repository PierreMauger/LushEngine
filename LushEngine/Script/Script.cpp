#include "Script/Script.hpp"

using namespace Lush;

Script::Script(std::string name)
{
    unsetenv("TERM");

    this->_domain = nullptr;
    this->_assembly = nullptr;
    this->_image = nullptr;

    this->_instance = nullptr;
    this->_onInit = nullptr;
    this->_onUpdate = nullptr;

    this->loadScript(name);
}

Script::~Script()
{
    mono_jit_cleanup(this->_domain);
}

void Script::loadScript(std::string name)
{
    std::string scriptPath = "Resources/Scripts/" + name + ".cs";
    std::string assemblyPath = "Resources/Scripts/" + name + ".dll";
    std::string command = "mcs " + scriptPath + " " + "Resources/Scripts/Transform.cs" + " " + "Resources/Scripts/InternalCalls.cs" + " -target:library";

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

    MonoClass *klass = mono_class_from_name(this->_image, "", name.c_str());
    if (!klass) {
        std::cout << "mono_class_from_name failed" << std::endl;
        return;
    }

    this->_instance = mono_object_new(this->_domain, klass);
    if (!this->_instance) {
        std::cout << "mono_object_new failed" << std::endl;
        return;
    }
    mono_runtime_object_init(this->_instance);

    this->_onInit = mono_class_get_method_from_name(klass, "onInit", 0);
    this->_onUpdate = mono_class_get_method_from_name(klass, "onUpdate", 1);
}

void Script::init()
{
    if (!this->_onInit)
        return;
    mono_runtime_invoke(this->_onInit, this->_instance, nullptr, nullptr);
}

void Script::update(float time)
{
    if (!this->_onUpdate)
        return;
    void *param = &time;
    mono_runtime_invoke(this->_onUpdate, this->_instance, &param, nullptr);
}
