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

            BufferObject _skybox{};
            BufferObject _billboard{};
            BufferObject _grid{};
            BufferObject _cameraFrustum{};
            BufferObject _sphere{};

            GLuint _perlinTexture = 0;

            void handleMouse();
            void generatePerlinTexture();
            void drawModels(Entity &entity, std::shared_ptr<EntityManager> &entityManager, const Transform &parentTransform = Transform());
            void drawBillboards(Entity &entity, std::shared_ptr<EntityManager> &entityManager, const Transform &parentTransform = Transform());
            void drawMap(std::shared_ptr<EntityManager> &entityManager);
            void drawSkybox(std::shared_ptr<EntityManager> &entityManager);
            void drawCameraFrustum(std::shared_ptr<EntityManager> &entityManager);
            void drawLightDirection(std::shared_ptr<EntityManager> &entityManager);
            void drawGrid();

        public:
            SceneSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~SceneSystem() override;

            void update(std::shared_ptr<EntityManager> &entityManager, float deltaTime) override;
    };
}

#endif // SCENE_SYSTEM_HPP
