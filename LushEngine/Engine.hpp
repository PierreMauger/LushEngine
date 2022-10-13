#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ComponentTypes.hpp"
#include "ECS/Component/ComponentManager.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "ECS/System/SystemManager.hpp"
#include "Includes.hpp"
#include "Systems/Control/ControlSystem.hpp"
#include "Systems/GUI/GUISystem.hpp"
#include "Systems/Render/RenderSystem.hpp"

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
