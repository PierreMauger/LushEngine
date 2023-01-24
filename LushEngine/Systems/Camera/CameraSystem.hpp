#ifndef CAMERASYSTEM_HPP
#define CAMERASYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"

#define CAMERA_TAG (ComponentType::TRANSFORM | ComponentType::CAMERA)
#define LIGHT_TAG (ComponentType::TRANSFORM | ComponentType::LIGHT)
#define CONTROL_TAG (ComponentType::TRANSFORM)

namespace Lush
{
    class CameraSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;

            std::vector<std::pair<Transform, Light>> _dirLights;
            std::vector<std::pair<Transform, Light>> _pointLights;
            std::vector<std::pair<Transform, Light>> _spotLights;
            // std::vector<std::pair<Transform, Light>> _areaLights;

        public:
            CameraSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~CameraSystem() = default;

            void update(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // CAMERASYSTEM_HPP
