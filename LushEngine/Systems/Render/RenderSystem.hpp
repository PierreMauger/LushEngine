#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include "ECS/Component/Component.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Shapes.hpp"
#include "ResourceManager.hpp"

namespace Lush
{
    class RenderSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

            BufferObject _skybox{};
            BufferObject _billboard{};

            void drawModel(Entity &entity, std::shared_ptr<EntityManager> &entityManager, const Transform &parentTransform = Transform());
            void drawBillboard(Entity &entity, std::shared_ptr<EntityManager> &entityManager, const Transform &parentTransform = Transform());
            void drawMap(std::shared_ptr<EntityManager> &entityManager);
            void drawSkybox(std::shared_ptr<EntityManager> &entityManager);

        public:
            RenderSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~RenderSystem() override;

            void update(std::shared_ptr<EntityManager> &entityManager, float deltaTime) override;
    };
}

#endif // RENDER_SYSTEM_HPP
