#include "File/Resource.hpp"

using namespace Lush;

static std::vector<Resource> resources;

bool Resource::hasFile(File file) const
{
    for (auto &f : this->_files) {
        // overload operator== for File
        if (f.getPath() == file.getPath())
            return true;
    }
    return false;
}

ResourceType Resource::getType() const
{
    return this->_type;
}

std::vector<Resource> &Resource::getResources()
{
    return resources;
}

bool Resource::operator==(const Resource &other) const
{
    return this->_name == other._name;
}
