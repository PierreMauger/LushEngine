#include "Rendering/Shapes.hpp"

using namespace Lush;

void Shapes::setupFrameBuffer(FrameBuffer &frameBuffer, glm::vec2 size)
{
    glGenFramebuffers(1, &frameBuffer.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.framebuffer);
    glGenTextures(1, &frameBuffer.texture);
    glBindTexture(GL_TEXTURE_2D, frameBuffer.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
}

void Shapes::setupCube(BufferObject &bufferObject)
{
    glGenVertexArrays(1, &bufferObject.vao);
    glGenBuffers(1, &bufferObject.vbo);
    glBindVertexArray(bufferObject.vao);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObject.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
}

void Shapes::deleteBufferObject(BufferObject &bufferObject)
{
    glBindVertexArray(bufferObject.vao);
    glDeleteBuffers(1, &bufferObject.vbo);
    glDeleteVertexArrays(1, &bufferObject.vao);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
