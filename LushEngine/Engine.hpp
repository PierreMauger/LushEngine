#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ComponentTypes.hpp"
#include "ECS/Component/ComponentManager.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "ECS/System/SystemManager.hpp"
#include "Graphic/Graphic.hpp"
#include "Includes.hpp"
#include "Systems/Camera/CameraSystem.hpp"
#include "Systems/Control/ControlSystem.hpp"
#include "Systems/GUI/GUISystem.hpp"
#include "Systems/Picking/PickingSystem.hpp"
#include "Systems/Render/RenderSystem.hpp"
#include "Systems/Scene/SceneSystem.hpp"
#include "Systems/Script/ScriptSystem.hpp"

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
            static Engine *getEngine();

            void loadScene();
            void run();
            void updateMouse();
            EntityManager &getEntityManager();
            ComponentManager &getComponentManager();
    };
}

#endif // ENGINE_HPP
