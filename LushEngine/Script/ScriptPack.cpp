#include "Script/ScriptPack.hpp"

using namespace Lush;

ScriptPack::ScriptPack(std::vector<File> &files, std::string name, std::unordered_map<std::string, ScriptPack> &corePacks) : Resource(ResourceType::SCRIPT, files)
{
    this->_name = name;
    try {
        this->load(files, corePacks);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void ScriptPack::load(std::vector<File> &files, std::unordered_map<std::string, ScriptPack> &corePacks)
{
    std::string assemblyPath = "Resources/bin/" + this->_name + ".dll";

    std::string command = "mcs -target:library -out:" + assemblyPath;
    for (auto &file : files)
        command += " " + file.getPath();
    for (auto &[name, corePack] : corePacks)
        command += " -r:Resources/bin/" + corePack.getName() + ".dll";

    if (system(command.c_str()))
        throw std::runtime_error("mcs failed");

    this->_domain = mono_domain_create_appdomain((char *)this->_name.c_str(), nullptr);

    this->_assembly = mono_domain_assembly_open(this->_domain, assemblyPath.c_str());
    if (!this->_assembly)
        throw std::runtime_error("mono_domain_assembly_open failed for " + this->_name + ".dll");

    this->_image = mono_assembly_get_image(this->_assembly);
    if (!this->_image)
        throw std::runtime_error("mono_assembly_get_image failed for " + this->_name + ".dll");

    for (auto &file : files) {
        MonoClass *klass = mono_class_from_name(this->_image, "", file.getName().c_str());
        if (!klass)
            throw std::runtime_error("mono_class_from_name failed for " + file.getName());
        this->_classes[file.getName()] = klass;
    }

    if (corePacks.find("Core") != corePacks.end()) {
        this->_coreClass = corePacks["Core"].getClasses()["Entity"];
        if (!this->_coreClass)
            throw std::runtime_error("mono_class_from_name failed for Entity in Core.dll");
    }
}

void ScriptPack::reload(std::vector<File> &files, std::unordered_map<std::string, ScriptPack> &corePacks)
{
    mono_domain_unload(this->_domain);
    this->load(files, corePacks);
}

std::string ScriptPack::getName() const
{
    return this->_name;
}

MonoDomain *ScriptPack::getDomain()
{
    return this->_domain;
}

std::unordered_map<std::string, MonoClass *> &ScriptPack::getClasses()
{
    return this->_classes;
}

MonoClass *ScriptPack::getCoreClass()
{
    return this->_coreClass;
}
