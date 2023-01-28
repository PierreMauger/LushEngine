#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Shapes.hpp"

#define MODEL_TAG (ComponentType::TRANSFORM | ComponentType::MODEL)
#define BILLBOARD_TAG (ComponentType::TRANSFORM | ComponentType::BILLBOARD)
#define SKYBOX_TAG (ComponentType::CUBEMAP)
#define MAP_TAG (ComponentType::MAP)

namespace Lush
{
    class RenderSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;

            FrameBuffer _buffer;
            BufferObject _skybox;
            BufferObject _billboard;

        public:
            RenderSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~RenderSystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime);
    };
}

#endif // RENDERSYSTEM_HPP
