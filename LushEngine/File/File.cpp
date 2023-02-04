#include "File.hpp"

using namespace Lush;

File::File(const std::string &path)
{
    this->_path = path;
    this->_lastModify = std::filesystem::last_write_time(path);
}

std::string File::getPath() const
{
    return this->_path;
}

std::string File::getName() const
{
    // get filename from path without extension
    return std::filesystem::path(this->_path).filename().replace_extension().string();
}

bool File::isModified() const
{
    return this->_lastModify != std::filesystem::last_write_time(this->_path);
}

void File::update()
{
    this->_lastModify = std::filesystem::last_write_time(this->_path);
}

std::string File::load() const
{
    std::ifstream file(this->_path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}
