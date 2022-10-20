#ifndef CAMERASYSTEM_HPP
#define CAMERASYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "Includes.hpp"

namespace Lush
{
    class CameraSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;

            std::vector<std::pair<Transform, Light>> _dirLights;
            std::vector<std::pair<Transform, Light>> _pointLights;
            std::vector<std::pair<Transform, Light>> _spotLights;
            // std::vector<Light> _areaLights;

        public:
            CameraSystem(std::shared_ptr<Graphic> graphic);
            ~CameraSystem() = default;

            void update(ComponentManager &componentManager, EntityManager &entityManager);
    };
}

#endif // CAMERASYSTEM_HPP
