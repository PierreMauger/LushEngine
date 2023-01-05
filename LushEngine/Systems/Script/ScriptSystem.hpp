#ifndef SCRIPTSYSTEM_HPP
#define SCRIPTSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "Includes.hpp"
#include "Script/ScriptClass.hpp"
#include "Script/ScriptGlue.hpp"
#include "Script/ScriptInstance.hpp"

namespace Lush
{
    class ScriptSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            // store unique domain? currently each script has a domain
            std::vector<std::unique_ptr<ScriptClass>> _scripts;
            std::vector<std::unique_ptr<ScriptInstance>> _instances;
            bool _initialized = false;

            bool buttonChanged();
            void loadBaseScript();

        public:
            ScriptSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~ScriptSystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // SCRIPTSYSTEM_HPP
