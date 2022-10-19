#ifndef GRAPHIC_HPP
#define GRAPHIC_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Graphic/Model.hpp"
#include "Graphic/RenderView.hpp"
#include "Graphic/Shader.hpp"
#include "Includes.hpp"

namespace Lush
{
    class Graphic
    {
        private:
            std::shared_ptr<GLFWwindow> _window;
            std::map<std::string, Shader> _shaders;
            std::map<std::size_t, Model> _models;
            RenderView _camera;

        public:
            Graphic();
            ~Graphic() = default;
            void setupWindow();

            std::shared_ptr<GLFWwindow> getWindow();
            std::map<std::string, Shader> &getShaders();
            std::map<std::size_t, Model> &getModels();
            RenderView &getCamera();
    };
}

#endif // GRAPHIC_HPP
