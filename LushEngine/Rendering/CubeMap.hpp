#ifndef CUBE_MAP_HPP
#define CUBE_MAP_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "File/File.hpp"
#include "Includes.hpp"
#include "STB/stb_image.h"

namespace Lush
{
    class CubeMap
    {
        private:
            unsigned int _id = 0;
            int _width = 0;
            int _height = 0;
            int _nrChannels = 0;

        public:
            explicit CubeMap(std::vector<File> &files);
            CubeMap() = default;
            ~CubeMap() = default;

            void load(std::vector<File> &files);

            [[nodiscard]] unsigned int getId() const;
    };
}

#endif // CUBE_MAP_HPP
