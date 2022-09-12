#ifndef RENDER_HPP
#define RENDER_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "AnimatedModel.hpp"
#include "Camera.hpp"
#include "Includes.hpp"
#include "Map.hpp"
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
            unsigned int _planeVAO;
            unsigned int _planeVBO;
            unsigned int _skyBoxVAO;
            unsigned int _skyBoxVBO;
            unsigned int _skyBoxTexture;

            float _deltaTime;
            float _lastFrame;

            std::map<std::string, std::shared_ptr<Shader>> _shaders;
            std::unique_ptr<Camera> _camera;
            bool _showCameraImGui;
            bool _showWindowImGui;
            int _selectionImGui;
            std::vector<std::string> _scenes;
            std::vector<glm::vec3> _pointLights;
            float _dirLightAngle;
            std::unique_ptr<Map> _map;

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
            void receiveLoadSkyBox(Packet);
            void receiveLoadModels(Packet);
            void receiveAddObject(Packet);
            void receiveClearObject(Packet);
            void receiveScenesName(Packet);

        private:
            bool showImGui(bool *open);
            void drawImGui();
            void drawScene();
            void drawMap();
            void drawPicking();
            void drawOutline();
            void drawSkyBox();
    };
}

#endif // RENDER_HPP
