#ifndef GRAPHIC_HPP
#define GRAPHIC_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Includes.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/CubeMap.hpp"
#include "Rendering/RenderModel.hpp"
#include "Rendering/RenderView.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Map.hpp"
#include "Script/ScriptClass.hpp"
#include "Script/ScriptInstance.hpp"
#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Scene.hpp"

namespace Lush
{
    class Graphic
    {
        private:
            GLFWwindow *_window;

            RenderView _renderView;
            std::unordered_map<std::string, FrameBuffer> _frameBuffers;

            std::ostringstream _stringStream;

            glm::vec4 _gameViewPort;
            glm::vec4 _sceneViewPort;
            glm::vec2 _windowSize;

            bool _drawWireframe = false;

            bool _running = false;
            bool _paused = false;

            std::size_t _hoveredEntity = 0;
            std::size_t _selectedEntity = -1;

            bool _mouseMovement = false;
            bool _sceneMovement = false;
            int _mouseButton = -1;
            glm::vec2 _mousePosition;
            glm::vec2 _mouseLastPosition;
            glm::vec2 _mouseOffset;

            std::array<GLFWcursor *, 3> _cursors;

        public:
            Graphic(int sizeX, int sizeY, std::string title);
            ~Graphic();
            void setGLFWContext(int sizeX, int sizeY, std::string title);

            void setCallBacks();
            void handleKeyboardPress(int key, int scancode, int action, int mods);
            void handleResizeFramebuffer(int width, int height);
            void handleMousePress(int button, int action, int mods);

            GLFWwindow *getWindow();

            RenderView &getRenderView();
            std::unordered_map<std::string, FrameBuffer> &getFrameBuffers();
            FrameBuffer &getFrameBuffer(std::string name);

            std::ostringstream &getStringStream();

            void setWireframe(bool drawWireframe);
            bool isWireframe();

            void setRunning(bool running);
            bool getRunning();
            void setPaused(bool paused);
            bool getPaused();

            void setHoveredEntity(std::size_t hoveredEntity);
            std::size_t getHoveredEntity();
            void setSelectedEntity(std::size_t selectedEntity);
            std::size_t getSelectedEntity();

            void setMouseMovement(bool mouseMovement);
            bool getMouseMovement();
            void setSceneMovement(bool sceneMovement);
            bool getSceneMovement();
            void setMouseButton(int mouseButton);
            int getMouseButton();
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
