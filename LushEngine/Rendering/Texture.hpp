#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "File.hpp"
#include "Includes.hpp"
#include "STB/stb_image.h"

namespace Lush
{
    class Texture
    {
        public:
            Texture(File &file);
            Texture() = default;
            ~Texture();

            void load(File &file);

            unsigned int getId();

        private:
            unsigned int _id = 0;
            int _width;
            int _height;
            int _nrChannels;
    };
}

#endif // TEXTURE_HPP
