#ifndef SCRIPTSYSTEM_HPP
#define SCRIPTSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "ResourceManager.hpp"
#include "Script/ScriptClass.hpp"
#include "Script/ScriptGlue.hpp"
#include "Script/ScriptInstance.hpp"

namespace Lush
{
    class ScriptSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

        public:
            ScriptSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~ScriptSystem() = default;

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime);
    };
}

#endif // SCRIPTSYSTEM_HPP
