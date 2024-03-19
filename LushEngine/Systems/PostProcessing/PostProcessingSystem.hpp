#ifndef POST_PROCESSING_SYSTEM_HPP
#define POST_PROCESSING_SYSTEM_HPP

#include "ECS/Component/Component.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Shapes.hpp"
#include "ResourceManager.hpp"

namespace Lush
{
    class PostProcessingSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

            BufferObject _screen{};

        public:
            PostProcessingSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~PostProcessingSystem() override = default;

            void update(std::shared_ptr<EntityManager> &entityManager, float deltaTime) override;
    };
}

#endif // POST_PROCESSING_SYSTEM_HPP
