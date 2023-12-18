#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "File/File.hpp"
#include "Includes.hpp"
#include "UUID.hpp"

namespace Lush
{
    enum class ResourceType {
        MODEL,
        TEXTURE,
        SKYBOX,
        SHADER,
        SCRIPT,
        SCENE,
        UNKNOWN
    };

    class Resource
    {
        private:
            UUID _uuid;
            ResourceType _type = ResourceType::UNKNOWN;
            std::vector<File> _files;
            bool _isUsed = true;

        public:
            template <typename... Args> explicit Resource(ResourceType type = ResourceType::UNKNOWN, Args... args)
            {
                this->_type = type;
                this->_files = {args...};
                this->_uuid = UUID();
                getResources().push_back(*this);
            }

            Resource() = default;
            virtual ~Resource() = default;

            bool hasFile(const File &file) const;
            void setFiles(std::vector<File> &files);
            std::vector<File> &getFiles();
            void setUsed(bool used);
            [[nodiscard]] bool isUsed() const;
            [[nodiscard]] std::string getUUID() const;
            [[nodiscard]] ResourceType getType() const;

            bool operator==(const Resource &other) const;

            static std::vector<Resource> &getResources();
    };
}

#endif // RESOURCE_HPP
