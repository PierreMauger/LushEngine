#ifndef PICKINGSYSTEM_HPP
#define PICKINGSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "Graphic/Vertices.hpp"
#include "Includes.hpp"

namespace Lush
{
    class PickingSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::size_t _modelTag = (ComponentType::TRANSFORM | ComponentType::MODEL);

            FrameBuffer _buffer;

            unsigned int _planeVAO;
            unsigned int _planeVBO;

        public:
            PickingSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~PickingSystem() = default;

            void update(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // PICKINGSYSTEM_HPP
