#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Includes.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/Vertices.hpp"

namespace Lush
{
    class Shapes
    {
        public:
            static void setupFrameBuffer(FrameBuffer &frameBuffer, glm::vec2 size);
            static void setupDepthBuffer(FrameBuffer &frameBuffer, glm::vec2 size);
            static void deleteFrameBuffer(FrameBuffer &frameBuffer);
            static void setupBillboard(BufferObject &bufferObject);
            static void setupPlane(BufferObject &bufferObject);
            static void setupSkybox(BufferObject &bufferObject);
            static void setupCube(BufferObject &bufferObject);
            static void deleteBufferObject(BufferObject &bufferObject);
    };
}

#endif // SHAPES_HPP
