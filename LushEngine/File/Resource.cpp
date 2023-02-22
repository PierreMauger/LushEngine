#include "File/Resource.hpp"

using namespace Lush;

static std::vector<Resource> resources;

std::vector<Resource> &Resource::getResources()
{
    return resources;
}

bool Resource::hasFile(File file) const
{
    for (auto &f : this->_files)
        if (f.getPath() == file.getPath())
            return true;
    return false;
}

std::vector<File> &Resource::getFiles()
{
    return this->_files;
}

std::string Resource::getUUID() const
{
    return this->_uuid.to_string();
}

ResourceType Resource::getType() const
{
    return this->_type;
}

bool Resource::operator==(const Resource &other) const
{
    return this->_uuid == other._uuid;
}
