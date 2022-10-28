#ifndef CONTROLSYSTEM_HPP
#define CONTROLSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "Includes.hpp"

namespace Lush
{
    class ControlSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;

        public:
            ControlSystem(std::shared_ptr<Graphic> graphic);
            ~ControlSystem() = default;

            void update(ComponentManager &componentManager, EntityManager &entityManager);
            void handleKeyboardPress(int key, int scancode, int action, int mods);
    };
}

#endif // CONTROLSYSTEM_HPP
