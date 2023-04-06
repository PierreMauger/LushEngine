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
            [[nodiscard]] bool isModified() const;
            void update();

            [[nodiscard]] std::string load() const;
    };
}

#endif // FILE_HPP
