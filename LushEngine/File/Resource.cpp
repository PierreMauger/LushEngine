#include "File/Resource.hpp"

using namespace Lush;

static std::vector<Resource> resources;

std::vector<Resource> &Resource::getResources()
{
    return resources;
}

bool Resource::hasFile(File &file) const
{
    return std::ranges::any_of(this->_files, [&file](const File &f) { return f.getPath() == file.getPath(); });
}

std::vector<File> &Resource::getFiles()
{
    return this->_files;
}

std::string Resource::getUUID() const
{
    return this->_uuid.toString();
}

ResourceType Resource::getType() const
{
    return this->_type;
}

bool Resource::operator==(const Resource &other) const
{
    return this->_uuid == other._uuid;
}
