#include "Script/ScriptPack.hpp"

using namespace Lush;

ScriptPack::ScriptPack(std::vector<File> &files, std::string name) : Resource(ResourceType::SCRIPT, files)
{
    this->_name = name;
    try {
        this->load(files);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void ScriptPack::load(std::vector<File> &files)
{
    std::string assemblyPath = "Resources/Scripts/" + this->_name + ".dll";

    std::string command = "mcs -target:library -out:" + assemblyPath;
    for (auto &file : files)
        command += " Resources/Scripts/" + file.getName() + ".cs";
    command += " -r:Resources/CoreScripts/Core.dll";
    if (system(command.c_str()))
        throw std::runtime_error("mcs failed");

    this->_domain = mono_domain_create_appdomain((char *)this->_name.c_str(), nullptr);

    this->_assembly = mono_domain_assembly_open(this->_domain, assemblyPath.c_str());
    if (!this->_assembly)
        throw std::runtime_error("mono_domain_assembly_open failed for " + this->_name + ".dll");
    this->_coreAssembly = mono_domain_assembly_open(this->_domain, "Resources/CoreScripts/Core.dll");
    if (!this->_coreAssembly)
        throw std::runtime_error("mono_domain_assembly_open failed for Core.dll");

    this->_image = mono_assembly_get_image(this->_assembly);
    if (!this->_image)
        throw std::runtime_error("mono_assembly_get_image failed for " + this->_name + ".dll");
    this->_coreImage = mono_assembly_get_image(this->_coreAssembly);
    if (!this->_coreImage)
        throw std::runtime_error("mono_assembly_get_image failed for Core.dll");

    for (auto &file : files) {
        MonoClass *klass = mono_class_from_name(this->_image, "", file.getName().c_str());
        if (!klass)
            throw std::runtime_error("mono_class_from_name failed for " + file.getName());
        this->_classes[file.getName()] = klass;
    }
    this->_coreClass = mono_class_from_name(this->_coreImage, "", "Entity");
    if (!this->_coreClass)
        throw std::runtime_error("mono_class_from_name failed for Entity in Core.dll");
}

void ScriptPack::reload(std::vector<File> &files)
{
    mono_domain_unload(this->_domain);
    this->load(files);
}

std::string ScriptPack::getName() const
{
    return this->_name;
}

MonoDomain *ScriptPack::getDomain()
{
    return this->_domain;
}

std::map<std::string, MonoClass *> &ScriptPack::getClasses()
{
    return this->_classes;
}

MonoClass *ScriptPack::getCoreClass()
{
    return this->_coreClass;
}
