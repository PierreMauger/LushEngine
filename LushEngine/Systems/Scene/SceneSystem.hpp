#ifndef SCENE_SYSTEM_HPP
#define SCENE_SYSTEM_HPP

#include <glm/gtc/noise.hpp>

#include "ECS/Component/Component.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Rendering/Shapes.hpp"
#include "ResourceManager.hpp"

namespace Lush
{
    class SceneSystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

            FrameBuffer _buffer{};
            BufferObject _skybox{};
            BufferObject _billboard{};
            BufferObject _grid{};
            BufferObject _cameraFrustum{};

            GLuint _perlinTexture = 0;

            Transform _cameraTransform;
            Camera _camera;

            void handleMouse();
            void generatePerlinTexture();
            void drawModels(EntityManager &entityManager);
            void drawBillboards(EntityManager &entityManager);
            void drawMap(EntityManager &entityManager);
            void drawSkybox(EntityManager &entityManager);
            void drawGrid();

        public:
            SceneSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~SceneSystem() override;

            void update(EntityManager &entityManager, float deltaTime) override;
    };
}

#endif // SCENE_SYSTEM_HPP
