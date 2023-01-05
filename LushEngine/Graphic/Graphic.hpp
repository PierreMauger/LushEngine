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
            GLFWwindow *_window;
            float _lastTime = 0.0f;

            std::map<std::string, Shader> _shaders;
            std::map<std::string, unsigned int> _textures;
            std::map<std::size_t, RenderModel> _models;
            std::map<std::size_t, unsigned int> _skyboxes;
            RenderView _renderView;
            std::vector<FrameBuffer> _frameBuffers;

            std::vector<std::string> _scriptNames;

            std::ostringstream _stringStream;

            glm::vec4 _gameViewPort;
            glm::vec4 _sceneViewPort;
            glm::vec2 _windowSize;

            bool _running = false;
            bool _paused = false;

            bool _mouseMovement = false;
            glm::vec2 _mousePosition;
            glm::vec2 _mouseLastPosition;
            glm::vec2 _mouseOffset;

        public:
            Graphic(int sizeX, int sizeY, std::string title);
            ~Graphic();
            void setGLFWContext(int sizeX, int sizeY, std::string title);

            void setCallBacks();
            void handleKeyboardPress(int key, int scancode, int action, int mods);
            void handleResizeFramebuffer(int width, int height);

            GLFWwindow *getWindow();
            float getLastTime();
            void setLastTime(float lastTime);

            std::map<std::string, Shader> &getShaders();
            std::map<std::string, unsigned int> &getTextures();
            std::map<std::size_t, RenderModel> &getModels();
            std::map<std::size_t, unsigned int> &getSkyboxes();
            RenderView &getRenderView();
            std::vector<FrameBuffer> &getFrameBuffers();
            std::vector<std::string> &getScriptNames();

            std::ostringstream &getStringStream();

            void setRunning(bool running);
            bool getRunning();
            void setPaused(bool paused);
            bool getPaused();

            void setMouseMovement(bool mouseMovement);
            bool getMouseMovement();
            void setMousePosition(glm::vec2 mousePosition);
            void setMouseOffset(glm::vec2 mousePosition);
            glm::vec2 getMousePosition();
            glm::vec2 getMouseOffset();

            void setGameViewPort(glm::vec4 viewPort);
            glm::vec4 getGameViewPort();
            void setSceneViewPort(glm::vec4 viewPort);
            glm::vec4 getSceneViewPort();
            void setWindowSize(glm::vec2 windowSize);
            glm::vec2 getWindowSize();
    };
}

#endif // GRAPHIC_HPP
