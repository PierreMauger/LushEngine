#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>

#include "Component/ComponentManager.hpp"
#include "Entity/EntityManager.hpp"
#include "GLFW/glfw3.h"
#include "Includes.hpp"
#include "System/Control/ControlSystem.hpp"
#include "System/GUI/GUISystem.hpp"
#include "System/Render/RenderSystem.hpp"
#include "System/SystemManager.hpp"

namespace Lush
{
    class Engine
    {
        private:
            EntityManager _entityManager;
            ComponentManager _componentManager;
            SystemManager _systemManager;

            std::shared_ptr<GLFWwindow> _window;

        public:
            Engine();
            ~Engine() = default;

            void initWindow();
            void run();
            void clear();
            void draw();
    };
}

#endif // ENGINE_HPP
