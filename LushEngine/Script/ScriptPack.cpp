#include "Script/ScriptPack.hpp"

using namespace Lush;

ScriptPack::ScriptPack(std::vector<File> &files, const std::string &name) : Resource(ResourceType::SCRIPT, files)
{
    this->_name = name;
    try {
        if (files.size() > 1)
            this->load(files);
        else
            this->loadFromAssembly(files[0].getPath());
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void ScriptPack::load(std::vector<File> &files)
{
    std::string assemblyPath = "Resources/bin/" + this->_name + ".dll";

    std::string command = "mcs -target:library -out:" + assemblyPath;
    for (auto &file : files)
        command += " \"" + file.getPath() + "\"";

    if (this->_name != "Core")
        command += " -r:Resources/bin/Core.dll";

    if (system(command.c_str()))
        throw std::runtime_error("mcs failed for " + command);

    this->_domain = mono_domain_create_appdomain((char *)this->_name.c_str(), nullptr);

    MonoAssembly *assembly = mono_domain_assembly_open(this->_domain, assemblyPath.c_str());
    if (!assembly)
        throw std::runtime_error("mono_domain_assembly_open failed for " + this->_name + ".dll");

    MonoImage *image = mono_assembly_get_image(assembly);
    if (!image)
        throw std::runtime_error("mono_assembly_get_image failed for " + this->_name + ".dll");

    for (auto &file : files) {
        MonoClass *klass = mono_class_from_name(image, "", file.getName().c_str());
        if (!klass)
            throw std::runtime_error("mono_class_from_name failed for " + file.getName());
        this->_classes[file.getName()] = klass;
    }
}

void ScriptPack::loadFromAssembly(const std::string &assemblyPath)
{
    this->_domain = mono_domain_create_appdomain((char *)this->_name.c_str(), nullptr);

    MonoAssembly *assembly = mono_domain_assembly_open(this->_domain, assemblyPath.c_str());
    if (!assembly)
        throw std::runtime_error("mono_domain_assembly_open failed for " + this->_name + ".dll");

    MonoImage *image = mono_assembly_get_image(assembly);
    if (!image)
        throw std::runtime_error("mono_assembly_get_image failed for " + this->_name + ".dll");

    const MonoTableInfo *table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int rows = mono_table_info_get_rows(table_info);

    for (int i = 0; i < rows; i++) {
        MonoClass *klass = nullptr;
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
        const char *name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
        const char *name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        if (std::string(name) == "<Module>")
            continue;
        klass = mono_class_from_name(image, name_space, name);
        if (!klass)
            throw std::runtime_error("mono_class_from_name failed for " + std::string(name));
        this->_classes[name] = klass;
    }
}

void ScriptPack::reload(std::vector<File> &files)
{
    mono_domain_unload(this->_domain);
    this->_classes.clear();

    this->load(files);
    for (auto &res : getResources())
        if (res == this)
            res->setFiles(files);
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
