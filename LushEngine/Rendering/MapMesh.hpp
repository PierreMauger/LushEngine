#ifndef MAP_MESH_HPP
#define MAP_MESH_HPP

#include "Includes.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Shader.hpp"

namespace Lush
{
    class MapMesh
    {
        private:
            BufferObject _map{};

            int _width;
            int _height;
            unsigned int _rez;

        public:
            MapMesh();
            ~MapMesh() = default;
            void draw() const;
    };
}

#endif // MAP_MESH_HPP
