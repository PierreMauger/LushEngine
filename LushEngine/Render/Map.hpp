#ifndef MAP_HPP
#define MAP_HPP

#include "Includes.hpp"
#include "Shader.hpp"

namespace Lush
{
    class Map
    {
        private:
            unsigned int _VAO;
            unsigned int _VBO;
            unsigned int _EBO;

            int _width;
            int _height;
            int _nrChannels;
            int _numStrips;
            int _numTrisPerStrip;

        public:
            Map(unsigned char *map, int width, int height, int nrChannels);
            ~Map() = default;
            void draw(Shader &shader);
    };
}

#endif // MAP_HPP
