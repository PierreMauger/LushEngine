#ifndef RESOURCE_HPP
#define RESOURCE_HPP

#include "File/File.hpp"
#include "Includes.hpp"

namespace Lush
{
    class Resource
    {
        private:
            std::vector<File> _files;

        public:
            Resource(std::vector<File> files);
            Resource() = default;
            ~Resource() = default;

            void reload();
            bool hasFile(File file) const;
    };
}

#endif // RESOURCE_HPP
