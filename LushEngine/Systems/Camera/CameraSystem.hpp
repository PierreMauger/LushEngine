#ifndef CAMERA_SYSTEM_HPP
#define CAMERA_SYSTEM_HPP

#include "ECS/Component/Component.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Shapes.hpp"
#include "ResourceManager.hpp"

namespace Lush
{
    class CameraSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

            FrameBuffer _lightBuffer{};

            std::vector<std::pair<Transform, Light>> _dirLights;
            std::vector<std::pair<Transform, Light>> _pointLights;
            std::vector<std::pair<Transform, Light>> _spotLights;
            // std::vector<std::pair<Transform, Light>> _areaLights;

        public:
            explicit CameraSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~CameraSystem() override = default;

            void update(std::shared_ptr<EntityManager> &entityManager, float deltaTime) override;
    };
}

#endif // CAMERA_SYSTEM_HPP
