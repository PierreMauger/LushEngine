#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "File/File.hpp"
#include "Includes.hpp"

namespace Lush
{
    enum class ResourceType
    {
        TEXTURE,
        MODEL,
        SHADER,
        SCRIPT,
        FONT,
        AUDIO,
        VIDEO,
        UNKNOWN
    };

    class Resource
    {
        private:
            std::string _name;
            std::vector<File> _files;
            ResourceType _type;

        public:
            template <typename... Args> Resource(std::string name, ResourceType type = ResourceType::UNKNOWN, Args... args)
            {
                this->_name = name;
                this->_type = type;
                this->_files = {args...};
                getResources().push_back(*this);
            }

            Resource() = default;
            ~Resource() = default;

            bool hasFile(File file) const;
            ResourceType getType() const;

            bool operator==(const Resource &other) const;

            static std::vector<Resource> &getResources();
    };
}

#endif // RESOURCE_HPP
