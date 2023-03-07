#ifndef SCENESYSTEM_HPP
#define SCENESYSTEM_HPP

#include <glm/gtc/noise.hpp>

#include "ComponentTypes.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Shapes.hpp"
#include "ResourceManager.hpp"

#define MODEL_TAG (ComponentType::TRANSFORM | ComponentType::MODEL)
#define BILLBOARD_TAG (ComponentType::TRANSFORM | ComponentType::BILLBOARD)
#define SKYBOX_TAG (ComponentType::CUBEMAP)
#define MAP_TAG (ComponentType::MAP)

namespace Lush
{
    class SceneSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

            FrameBuffer _buffer;
            BufferObject _skybox;
            BufferObject _billboard;
            BufferObject _grid;

            BufferObject _cameraFrustum;
            GLuint _prelinTexture;

            Transform _cameraTransform;
            Camera _camera;

            void handleMouse();
            void generatePerlinTexture();
            void drawModels(EntityManager &entityManager, ComponentManager &componentManager);
            void drawBillboards(EntityManager &entityManager, ComponentManager &componentManager);
            void drawMap(EntityManager &entityManager, ComponentManager &componentManager);
            void drawSkybox(EntityManager &entityManager, ComponentManager &componentManager);
            void drawGrid();

        public:
            SceneSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~SceneSystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime);
    };
}

#endif // SCENESYSTEM_HPP