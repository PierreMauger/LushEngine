#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "File.hpp"
#include "Includes.hpp"
#include "STB/stb_image.h"

namespace Lush
{
    class Texture
    {
        private:
            unsigned int _id = 0;
            int _width;
            int _height;
            int _nrChannels;
            std::vector<unsigned char> _pixels;
            unsigned char *_heightData = nullptr;
            std::string _content;

            void setupTexture();

        public:
            Texture(File &file);
            Texture() = default;
            ~Texture();

            void load(File &file);
            void createTexture();

            unsigned int getId() const;
            int getWidth() const;
            int getHeight() const;
            unsigned char *getData() const;

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &_content;
            }
    };
}

#endif // TEXTURE_HPP
