#ifndef GRAPHIC_HPP
#define GRAPHIC_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Graphic/RenderModel.hpp"
#include "Graphic/RenderView.hpp"
#include "Graphic/Shader.hpp"
#include "Includes.hpp"

namespace Lush
{
    typedef struct {
        GLuint framebuffer;
        GLuint texture;
        GLuint depthbuffer;
    } FrameBuffer;

    class Graphic
    {
        private:
            std::shared_ptr<GLFWwindow> _window;
            std::map<std::string, Shader> _shaders;
            std::map<std::size_t, RenderModel> _models;
            std::map<std::size_t, unsigned int> _skyboxes;
            RenderView _camera;

            glm::vec2 _mousePosition;
            glm::vec2 _mouseOffset;

        public:
            Graphic();
            ~Graphic() = default;
            void setupWindow();

            std::shared_ptr<GLFWwindow> getWindow();
            std::map<std::string, Shader> &getShaders();
            std::map<std::size_t, RenderModel> &getModels();
            std::map<std::size_t, unsigned int> &getSkyboxes();
            RenderView &getCamera();

            void setMousePosition(glm::vec2 mousePosition);
            glm::vec2 getMousePosition();
            glm::vec2 getMouseOffset();
    };
}

#endif // GRAPHIC_HPP
