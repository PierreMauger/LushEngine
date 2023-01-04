#ifndef CONTROLSYSTEM_HPP
#define CONTROLSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "Includes.hpp"

#define CONTROL_TAG (ComponentType::TRANSFORM | ComponentType::CONTROL)
#define CAMERA_TAG (ComponentType::TRANSFORM | ComponentType::CAMERA)

namespace Lush
{
    class ControlSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;

        public:
            ControlSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~ControlSystem() = default;

            void update(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // CONTROLSYSTEM_HPP
