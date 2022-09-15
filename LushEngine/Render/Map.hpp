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

            int _width;
            int _height;
            unsigned int _rez;

        public:
            Map(int width, int height);
            ~Map() = default;
            void draw();
    };
}

#endif // MAP_HPP
