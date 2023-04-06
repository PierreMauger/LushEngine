#ifndef PICKING_SYSTEM_HPP
#define PICKING_SYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Shapes.hpp"
#include "ResourceManager.hpp"

#define MODEL_TAG (ComponentType::TRANSFORM | ComponentType::MODEL)
#define BILLBOARD_TAG (ComponentType::TRANSFORM | ComponentType::BILLBOARD)

namespace Lush
{
    class PickingSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

            FrameBuffer _buffer{};
            BufferObject _billboard{};
            BufferObject _plane{};

            void drawModels(EntityManager &entityManager, ComponentManager &componentManager);
            void drawBillboards(EntityManager &entityManager, ComponentManager &componentManager);
            void drawOutline(std::size_t pixel);

        public:
            PickingSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~PickingSystem() override;

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime) override;
    };
}

#endif // PICKING_SYSTEM_HPP
