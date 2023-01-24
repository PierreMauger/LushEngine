#ifndef ECS_HPP
#define ECS_HPP

#include "ECS/Component/ComponentManager.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "ECS/System/SystemManager.hpp"
#include "Graphic.hpp"
#include "Systems/Camera/CameraSystem.hpp"
#include "Systems/GUI/GUISystem.hpp"
#include "Systems/Picking/PickingSystem.hpp"
#include "Systems/Render/RenderSystem.hpp"
#include "Systems/Scene/SceneSystem.hpp"
#include "Systems/Script/ScriptSystem.hpp"

namespace Lush
{
    class ECS
    {
        public:
            ECS();
            ~ECS() = default;

            void loadComponents();
            void loadSystems(std::shared_ptr<Graphic> graphic);

            EntityManager &getEntityManager();
            ComponentManager &getComponentManager();
            SystemManager &getSystemManager();

            static ECS *getECS();

        private:
            EntityManager _entityManager;
            ComponentManager _componentManager;
            SystemManager _systemManager;
    };
}

#endif // ECS_HPP
