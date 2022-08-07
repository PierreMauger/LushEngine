#ifndef RENDER_HPP
#define RENDER_HPP

#include <GL/glew.h>

#include "AnimatedModel.hpp"
#include "Camera.hpp"
#include "GLFW/glfw3.h"
#include "Includes.hpp"
#include "Node.hpp"
#include "RenderObject.hpp"
#include "StaticModel.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Lush
{
    class Render : public virtual Node
    {
        private:
            GLFWwindow *_window;
            std::unique_ptr<Camera> _camera;
            bool showImGuiCamera;
            std::map<int, std::unique_ptr<RenderObject>> _objects;

        public:
            Render(std::shared_ptr<MessageBus> messageBus);
            ~Render();
            void run();
            void draw();
    };
}

#endif // RENDER_HPP
