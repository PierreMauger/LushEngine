#include "Rendering/Shapes.hpp"

using namespace Lush;

void Shapes::setupFrameBuffer(FrameBuffer &frameBuffer, glm::vec2 size)
{
    glGenFramebuffers(1, &frameBuffer.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.framebuffer);
    glGenTextures(1, &frameBuffer.texture);
    glBindTexture(GL_TEXTURE_2D, frameBuffer.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.texture, 0);
    glGenRenderbuffers(1, &frameBuffer.depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer.depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBuffer.depthbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shapes::setupDepthBuffer(FrameBuffer &frameBuffer, glm::vec2 size)
{
    frameBuffer.resizable = false;
    glGenFramebuffers(1, &frameBuffer.depthbuffer);
    glGenTextures(1, &frameBuffer.texture);
    glBindTexture(GL_TEXTURE_2D, frameBuffer.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.depthbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frameBuffer.texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shapes::deleteFrameBuffer(FrameBuffer &frameBuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
    glDeleteFramebuffers(1, &frameBuffer.framebuffer);
    glDeleteTextures(1, &frameBuffer.texture);
    glDeleteRenderbuffers(1, &frameBuffer.depthbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Shapes::setupBillboard(BufferObject &bufferObject)
{
    glGenVertexArrays(1, &bufferObject.vao);
    glGenBuffers(1, &bufferObject.vbo);
    glBindVertexArray(bufferObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObject.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(billboardVertices), &billboardVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

void Shapes::setupPlane(BufferObject &bufferObject)
{
    glGenVertexArrays(1, &bufferObject.vao);
    glGenBuffers(1, &bufferObject.vbo);
    glBindVertexArray(bufferObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObject.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
}

void Shapes::setupSkybox(BufferObject &bufferObject)
{
    glGenVertexArrays(1, &bufferObject.vao);
    glGenBuffers(1, &bufferObject.vbo);
    glBindVertexArray(bufferObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObject.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)nullptr);
}

void Shapes::setupCube(BufferObject &bufferObject)
{
    glGenVertexArrays(1, &bufferObject.vao);
    glGenBuffers(1, &bufferObject.vbo);
    glBindVertexArray(bufferObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObject.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)nullptr);
}

void createCircle(std::vector<GLfloat> &circleVertices, int segments, char plane)
{
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * float(i) / float(segments);

        float x = cosf(angle);
        float y = sinf(angle);

        std::array<float, 3> vertex = {x, y, 0.0f};
        vertex[plane] = 0.0f;
        vertex[(plane + 1) % 3] = x;
        vertex[(plane + 2) % 3] = y;

        circleVertices.insert(circleVertices.end(), vertex.begin(), vertex.end());
    }
}

void Shapes::setupSphere(BufferObject &bufferObject, int segments)
{
    std::vector<float> sphereVertices;

    createCircle(sphereVertices, segments, 0);
    createCircle(sphereVertices, segments, 1);
    createCircle(sphereVertices, segments, 2);

    glGenVertexArrays(1, &bufferObject.vao);
    glGenBuffers(1, &bufferObject.vbo);
    glBindVertexArray(bufferObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObject.vbo);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)nullptr);
}

void Shapes::deleteBufferObject(BufferObject &bufferObject)
{
    glBindVertexArray(bufferObject.vao);
    glDeleteBuffers(1, &bufferObject.vbo);
    glDeleteVertexArrays(1, &bufferObject.vao);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
