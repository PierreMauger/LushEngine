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

            BufferObject _billboard{};
            BufferObject _plane{};

            void drawModel(Entity &entity, std::size_t id, std::shared_ptr<EntityManager> &entityManager, const Transform &parentTransform = Transform());
            void drawBillboard(Entity &entity, std::size_t id, std::shared_ptr<EntityManager> &entityManager, const Transform &parentTransform = Transform());
            void drawOutline(std::size_t pixel);

        public:
            PickingSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~PickingSystem() override;

            void update(std::shared_ptr<EntityManager> &entityManager, float deltaTime) override;
    };
}

#endif // PICKING_SYSTEM_HPP
