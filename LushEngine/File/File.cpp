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
    return std::filesystem::path(this->_path).filename().replace_extension().string();
}

std::string File::getExtension() const
{
    return std::filesystem::path(this->_path).extension().string();
}

bool File::isModified() const
{
    return this->_lastModify != std::filesystem::last_write_time(this->_path);
}

bool File::isDeleted() const
{
    return !std::filesystem::exists(this->_path);
}

void File::updateLastModify()
{
    this->_lastModify = std::filesystem::last_write_time(this->_path);
}

std::string File::load() const
{
    std::ifstream file(this->_path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

void File::save(const std::string &content) const
{
    std::ofstream file(this->_path);
    file << content;
    file.close();
}
