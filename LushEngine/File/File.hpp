#ifndef FILE_HPP
#define FILE_HPP

#include "Includes.hpp"

namespace Lush
{
    class File
    {
        private:
            std::string _path;
            std::filesystem::file_time_type _lastModify;

        public:
            explicit File(const std::string &path);
            File() = default;
            ~File() = default;

            [[nodiscard]] std::string getPath() const;
            [[nodiscard]] std::string getName() const;
            [[nodiscard]] std::string getExtension() const;
            [[nodiscard]] bool isModified() const;
            [[nodiscard]] bool isDeleted() const;
            void updateLastModify();

            [[nodiscard]] std::string load() const;
            void save(const std::string &content) const;
    };
}

#endif // FILE_HPP
