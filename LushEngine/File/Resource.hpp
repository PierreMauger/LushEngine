#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "File/File.hpp"
#include "Includes.hpp"
#include "UUID.hpp"

namespace Lush
{
    enum class ResourceType {
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
            UUID _uuid;
            ResourceType _type;
            std::vector<File> _files;

        public:
            template <typename... Args> Resource(ResourceType type = ResourceType::UNKNOWN, Args... args)
            {
                this->_type = type;
                this->_files = {args...};
                this->_uuid = UUID();
                getResources().push_back(*this);
            }

            Resource() = default;
            ~Resource() = default;

            bool hasFile(File file) const;
            std::vector<File> &getFiles();
            std::string getUUID() const;
            ResourceType getType() const;

            bool operator==(const Resource &other) const;

            static std::vector<Resource> &getResources();
    };
}

#endif // RESOURCE_HPP
