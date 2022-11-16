#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <lua.hpp>

#include "ComponentTypes.hpp"
#include "ECS/Component/ComponentManager.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "ECS/System/SystemManager.hpp"
#include "Graphic/Graphic.hpp"
#include "Includes.hpp"
#include "Systems/Control/ControlSystem.hpp"
#include "Systems/Camera/CameraSystem.hpp"
#include "Systems/GUI/GUISystem.hpp"
#include "Systems/Picking/PickingSystem.hpp"
#include "Systems/Render/RenderSystem.hpp"

namespace Lush
{
    class Engine
    {
        private:
            EntityManager _entityManager;
            ComponentManager _componentManager;
            SystemManager _systemManager;

            std::shared_ptr<Graphic> _graphic;

        public:
            Engine();
            ~Engine() = default;

            void loadScene();
            void run();
            void clear();
            void draw();
    };
}

#endif // ENGINE_HPP
