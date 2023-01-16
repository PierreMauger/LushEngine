#ifndef SCENESYSTEM_HPP
#define SCENESYSTEM_HPP

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
    class SceneSystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;

            FrameBuffer _buffer;

            unsigned int _skyboxVAO;
            unsigned int _skyboxVBO;

            unsigned int _billboardVAO;
            unsigned int _billboardVBO;

            unsigned int _gridVAO;
            unsigned int _gridVBO;

            Transform _cameraTransform;
            Camera _camera;

        public:
            SceneSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager);
            ~SceneSystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // SCENESYSTEM_HPP
