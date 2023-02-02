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
            File(std::string path);
            File() = default;
            ~File() = default;

            std::string getPath() const;
            std::string getName() const;
            bool isModified() const;
            void update();

            std::string load() const;
    };
}

#endif // FILE_HPP
