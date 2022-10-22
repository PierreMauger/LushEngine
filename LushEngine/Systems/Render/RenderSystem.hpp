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

            unsigned int _skyBoxVAO;
            unsigned int _skyBoxVBO;
            unsigned int _skyBoxTexture;

        public:
            RenderSystem(std::shared_ptr<Graphic> graphic);
            ~RenderSystem();

            void update(ComponentManager &componentManager, EntityManager &entityManager);
    };
}

#endif // RENDERSYSTEM_HPP