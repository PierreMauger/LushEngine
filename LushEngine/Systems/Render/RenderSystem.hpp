#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "Graphic/Vertices.hpp"
#include "Includes.hpp"

namespace Lush
{
    class RenderSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::size_t _modelTag = (ComponentType::TRANSFORM | ComponentType::MODEL);
            std::size_t _billboardTag = (ComponentType::TRANSFORM | ComponentType::BILLBOARD);
            std::size_t _skyboxTag = (ComponentType::CUBEMAP);

            FrameBuffer _buffer;

            unsigned int _skyboxVAO;
            unsigned int _skyboxVBO;

            unsigned int _billboardVAO;
            unsigned int _billboardVBO;

        public:
            RenderSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~RenderSystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // RENDERSYSTEM_HPP
