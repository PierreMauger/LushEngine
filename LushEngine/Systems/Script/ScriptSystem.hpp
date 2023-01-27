#ifndef SCRIPTSYSTEM_HPP
#define SCRIPTSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Script/ScriptClass.hpp"
#include "Script/ScriptGlue.hpp"
#include "Script/ScriptInstance.hpp"

namespace Lush
{
    class ScriptSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::vector<std::unique_ptr<ScriptClass>> _scripts;
            std::vector<std::unique_ptr<ScriptInstance>> _instances;
            bool _initialized = false;
            MonoDomain *_domain;

            bool buttonChanged();
            void initScriptDomain();
            void loadBaseScript();

        public:
            ScriptSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~ScriptSystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime);
    };
}

#endif // SCRIPTSYSTEM_HPP
