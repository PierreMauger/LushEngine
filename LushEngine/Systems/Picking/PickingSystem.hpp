#ifndef PICKINGSYSTEM_HPP
#define PICKINGSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "Includes.hpp"

namespace Lush
{
    class PickingSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            FrameBuffer _buffer;

            unsigned int _planeVAO;
            unsigned int _planeVBO;

        public:
            PickingSystem(std::shared_ptr<Graphic> graphic);
            ~PickingSystem() = default;

            void update(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // PICKINGSYSTEM_HPP
