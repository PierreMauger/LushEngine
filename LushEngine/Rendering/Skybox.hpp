#ifndef CUBE_MAP_HPP
#define CUBE_MAP_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "STB/stb_image.h"

namespace Lush
{
    class Skybox : public Resource
    {
        private:
            unsigned int _id = 0;
            int _width = 0;
            int _height = 0;
            int _nrChannels = 0;
            std::string _content;

        public:
            Skybox(File &file);
            Skybox() = default;
            ~Skybox() = default;

            void load(const File &file);

            void createSkybox();

            [[nodiscard]] unsigned int getId() const;

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar & _content;
            }
    };
}

#endif // CUBE_MAP_HPP
