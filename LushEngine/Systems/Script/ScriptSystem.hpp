#ifndef SCRIPTSYSTEM_HPP
#define SCRIPTSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "Includes.hpp"
#include "LuaScript/LuaScript.hpp"

namespace Lush
{
    class ScriptSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::vector<LuaScript> _scripts;

        public:
            ScriptSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~ScriptSystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // SCRIPTSYSTEM_HPP
