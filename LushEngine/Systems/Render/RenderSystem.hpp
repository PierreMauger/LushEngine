#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Vertices.hpp"

#define MODEL_TAG (ComponentType::TRANSFORM | ComponentType::MODEL)
#define BILLBOARD_TAG (ComponentType::TRANSFORM | ComponentType::BILLBOARD)
#define SKYBOX_TAG (ComponentType::CUBEMAP)

namespace Lush
{
    class RenderSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;

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
