#ifndef PICKING_SYSTEM_HPP
#define PICKING_SYSTEM_HPP

#include "ECS/Component/Component.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Shapes.hpp"
#include "ResourceManager.hpp"

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

            void drawModels(EntityManager &entityManager);
            void drawBillboards(EntityManager &entityManager);
            void drawOutline(std::size_t pixel);

        public:
            PickingSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~PickingSystem() override;

            void update(EntityManager &entityManager, float deltaTime) override;
    };
}

#endif // PICKING_SYSTEM_HPP
