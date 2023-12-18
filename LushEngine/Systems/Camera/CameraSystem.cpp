#include "Systems/Camera/CameraSystem.hpp"

using namespace Lush;

CameraSystem::CameraSystem(std::shared_ptr<Graphic> graphic) : ASystem(60.0f), _graphic(graphic)
{
}

void CameraSystem::update(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    for (auto &[id, entity] : entityManager->getEntities()) {
        if (entity.hasComponent<Transform>() && entity.hasComponent<Camera>())
            this->_graphic->getRenderView().update(entity.getComponent<Transform>(), entity.getComponent<Camera>());
        if (entity.hasComponent<Transform>() && entity.hasComponent<Light>()) {
            auto &transform = entity.getComponent<Transform>();
            auto &light = entity.getComponent<Light>();

            if (light.type == LightType::DIRECTIONAL)
                this->_dirLights.emplace_back(transform, light);
            else if (light.type == LightType::POINT)
                this->_pointLights.emplace_back(transform, light);
            else if (light.type == LightType::SPOT)
                this->_spotLights.emplace_back(transform, light);
        }
    }

    this->_graphic->getRenderView().use("Model");
    this->_graphic->getRenderView().setDirLights(this->_dirLights);
    this->_graphic->getRenderView().setPointLights(this->_pointLights);
    this->_graphic->getRenderView().use("Map");
    this->_graphic->getRenderView().setDirLights(this->_dirLights);
    this->_graphic->getRenderView().setPointLights(this->_pointLights);

    this->_dirLights.clear();
    this->_pointLights.clear();
    this->_spotLights.clear();
}
