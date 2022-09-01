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
    typedef struct {
            GLuint framebuffer;
            GLuint texture;
            GLuint depthbuffer;
    } FrameBuffer;

    class Render : public virtual Node
    {
        private:
            GLFWwindow *_window;
            double _mouseX;
            double _mouseY;
            float _lastMouseX;
            float _lastMouseY;
            bool _freeCamera;
            double _windowWidth;
            double _windowHeight;
            FrameBuffer _hoverBuffer;
            int _hover;
            unsigned int _quadVAO;
            unsigned int _quadVBO;

            float _deltaTime;
            float _lastFrame;

            std::map<std::string, std::shared_ptr<Shader>> _shaders;
            std::unique_ptr<Camera> _camera;
            bool _showCameraImGui;
            bool _showWindowImGui;
            int _selectionImGui;
            std::vector<std::string> _scenes;

            std::map<std::string, unsigned int> _textures;
            std::map<std::string, std::shared_ptr<Model>> _models;
            std::map<std::size_t, std::unique_ptr<RenderObject>> _objects;

        public:
            Render(std::shared_ptr<MessageBus> messageBus);
            ~Render();
            void run();
            void update();
            void draw();
            void handleTime();

            void handleMouseMovement();
            void handleMouseClick();
            void handleKeyboard();
            void handleKeyboardPress(int key, int scancode, int action, int mods);

            void receiveLoadIcon(Packet);
            void receiveLoadShaders(Packet);
            void receiveLoadTextures(Packet);
            void receiveLoadModels(Packet);
            void receiveAddObject(Packet);
            void receiveClearObject(Packet);
            void receiveScenesName(Packet);

        private:
            bool showImGui(bool *open);
            void drawImGui();
            void drawScene();
            void drawPicking();
            void drawOutline();
    };
}

#endif // RENDER_HPP
