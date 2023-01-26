#ifndef MAP_HPP
#define MAP_HPP

#include "Includes.hpp"
#include "Rendering/Shader.hpp"

namespace Lush
{
    class MapMesh
    {
        private:
            unsigned int _VAO;
            unsigned int _VBO;

            int _width;
            int _height;
            unsigned int _rez;

        public:
            MapMesh(int width, int height);
            ~MapMesh() = default;
            void draw();
    };
}

#endif // MAP_HPP
