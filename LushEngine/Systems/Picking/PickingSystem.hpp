#ifndef PICKINGSYSTEM_HPP
#define PICKINGSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Vertices.hpp"

#define MODEL_TAG (ComponentType::TRANSFORM | ComponentType::MODEL)
#define BILLBOARD_TAG (ComponentType::TRANSFORM | ComponentType::BILLBOARD)

namespace Lush
{
    class PickingSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;

            FrameBuffer _buffer;
            BufferObject _billboard;
            BufferObject _plane;

        public:
            PickingSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~PickingSystem() = default;

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime);
    };
}

#endif // PICKINGSYSTEM_HPP
