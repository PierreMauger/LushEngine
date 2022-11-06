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
            std::size_t _controlTag = (ComponentType::TRANSFORM | ComponentType::CONTROL);
            std::size_t _cameraTag = (ComponentType::TRANSFORM | ComponentType::CAMERA);

        public:
            ControlSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~ControlSystem() = default;

            void update(EntityManager &entityManager, ComponentManager &componentManager);
            void handleKeyboardPress(int key, int scancode, int action, int mods);
    };
}

#endif // CONTROLSYSTEM_HPP
