#ifndef GRAPHIC_HPP
#define GRAPHIC_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "Rendering/CubeMap.hpp"
#include "Rendering/MapMesh.hpp"
#include "Rendering/RenderModel.hpp"
#include "Rendering/RenderView.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"
#include "Scene.hpp"

namespace Lush
{
    class Graphic
    {
        private:
            GLFWwindow *_window{};
            RenderView _renderView;
            std::unordered_map<std::string, FrameBuffer> _frameBuffers;
            std::ostringstream _stringStream;

            glm::vec4 _gameViewPort{};
            glm::vec4 _sceneViewPort{};
            glm::vec2 _windowSize{};

            bool _drawWireframe = false;
            bool _running = false;
            bool _paused = false;

            std::size_t _hoveredEntity = 0;
            std::size_t _selectedEntity = -1;

            bool _mouseHidden = false;
            bool _sceneHovered = false;
            int _mouseButton = -1;
            glm::vec2 _mousePosition{};
            glm::vec2 _mouseLastPosition{};
            glm::vec2 _mouseOffset{};

            std::array<GLFWcursor *, 3> _cursors{};

            void setGLFWContext(int sizeX, int sizeY, const std::string &title);
            void setCallBacks();
            void handleKeyboardPress(int key, int scancode, int action, int mods);
            void handleResizeFramebuffer(int width, int height);
            void handleMousePress(int button, int action, int mods);

        public:
            Graphic(int sizeX, int sizeY, const std::string &title);
            ~Graphic();

            GLFWwindow *getWindow();
            RenderView &getRenderView();
            std::unordered_map<std::string, FrameBuffer> &getFrameBuffers();
            std::ostringstream &getStringStream();

            void setWireframe(bool drawWireframe);
            bool isWireframe() const;

            void setRunning(bool running);
            bool isRunning() const;
            void setPaused(bool paused);
            bool isPaused() const;

            void setHoveredEntity(std::size_t hoveredEntity);
            void setSelectedEntity(std::size_t selectedEntity);
            std::size_t getSelectedEntity() const;

            bool isMouseHidden() const;
            void setSceneHovered(bool sceneHovered);
            bool isSceneHovered() const;
            int getMouseButton() const;
            void setMousePosition(glm::vec2 mousePosition);
            glm::vec2 getMousePosition() const;
            void setMouseOffset(glm::vec2 mousePosition);
            glm::vec2 getMouseOffset() const;

            void setGameViewPort(glm::vec4 viewPort);
            glm::vec4 getGameViewPort() const;
            void setSceneViewPort(glm::vec4 viewPort);
            glm::vec4 getSceneViewPort() const;
//            void setWindowSize(glm::vec2 windowSize);
            glm::vec2 getWindowSize() const;

            void setLogo(Texture &logo);

            static Graphic *getGraphic();
    };
}

#endif // GRAPHIC_HPP
