#ifndef CUBEMAP_HPP
#define CUBEMAP_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "File.hpp"
#include "Includes.hpp"
#include "STB/stb_image.h"

namespace Lush
{
    class CubeMap
    {
        public:
            CubeMap(std::vector<File> &files);
            CubeMap() = default;
            ~CubeMap();

            void load(std::vector<File> &files);

            unsigned int getId();

        private:
            unsigned int _id = 0;
            int _width;
            int _height;
            int _nrChannels;
    };
}

#endif // CUBEMAP_HPP
