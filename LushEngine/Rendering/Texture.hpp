#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "STB/stb_image.h"

namespace Lush
{
    class Texture : public Resource
    {
        private:
            unsigned int _id = 0;
            int _width;
            int _height;
            int _nrChannels;
            unsigned char *_heightData = nullptr;
            std::string _content;

        public:
            explicit Texture(File &file);
            Texture() = default;
            ~Texture();

            void load(const File &file);
            void reload(const File &file);
            void createTexture();

            [[nodiscard]] unsigned int getId() const;
            [[nodiscard]] int getWidth() const;
            [[nodiscard]] int getHeight() const;
            [[nodiscard]] unsigned char *getHeightData() const;
            [[nodiscard]] std::string getContent() const;

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &_content;
            }
    };
}

#endif // TEXTURE_HPP
