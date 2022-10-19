#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "Includes.hpp"

namespace Lush
{
    class RenderSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;

            std::vector<std::pair<Transform, Light>> _dirLights;
            std::vector<std::pair<Transform, Light>> _pointLights;
            std::vector<std::pair<Transform, Light>> _spotLights;
            // std::vector<Light> _areaLights;

        public:
            RenderSystem(std::shared_ptr<Graphic> graphic);
            ~RenderSystem();

            void update(ComponentManager &componentManager, EntityManager &entityManager);
    };
}

#endif // RENDERSYSTEM_HPP
