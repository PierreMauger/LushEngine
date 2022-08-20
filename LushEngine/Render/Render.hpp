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
            double _mouseX;
            double _mouseY;
            double _windowWidth;
            double _windowHeight;
            GLuint _framebuffer;
            GLuint _texture;
            GLuint _depthbuffer;
            int _hover;

            std::map<std::string, std::shared_ptr<Shader>> _shaders;
            std::unique_ptr<Camera> _camera;
            bool showImGuiCamera;

            std::map<int, std::unique_ptr<RenderObject>> _objects;

        public:
            Render(std::shared_ptr<MessageBus> messageBus);
            ~Render();
            void run();
            void draw();
            void handleMouse();

            void receiveLoadShader(Packet);

        private:
            void drawImGui();
            void drawScene();
            void drawPicking();
    };
}

#endif // RENDER_HPP
