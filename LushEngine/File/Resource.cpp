#include "File/Resource.hpp"

using namespace Lush;

static std::vector<Resource> resources;

std::vector<Resource> &Resource::getResources()
{
    return resources;
}

bool Resource::hasFile(const File &file) const
{
    return std::ranges::any_of(this->_files, [&file](const File &f) { return f.getPath() == file.getPath(); });
}

void Resource::setFiles(std::vector<File> &files)
{
    this->_files = files;
}

std::vector<File> &Resource::getFiles()
{
    return this->_files;
}

void Resource::setUsed(bool used)
{
    this->_isUsed = used;
}

bool Resource::isUsed() const
{
    return this->_isUsed;
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
