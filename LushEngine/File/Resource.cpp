#include "File/Resource.hpp"

using namespace Lush;

Resource::Resource(std::vector<File> files)
{
    this->_files = files;
}

void Resource::reload()
{
    std::cout << "Reloading resource" << std::endl;
}

bool Resource::hasFile(File file) const
{
    for (auto &f : this->_files) {
        // overload operator== for File
        if (f.getPath() == file.getPath())
            return true;
    }
    return false;
}
