#include "Systems/Camera/CameraSystem.hpp"

using namespace Lush;

CameraSystem::CameraSystem(std::shared_ptr<Graphic> graphic) : ASystem(60.0f), _graphic(std::move(graphic))
{
}

void CameraSystem::update(EntityManager &entityManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Camera>())
            continue;
        auto &transform = entity.getComponent<Transform>();
        auto &camera = entity.getComponent<Camera>();

        this->_graphic->getRenderView().update(transform, camera);
    }
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Light>())
            continue;
        auto &transform = entity.getComponent<Transform>();
        auto &light = entity.getComponent<Light>();

        if (light.type == LightType::DIRECTIONAL)
            this->_dirLights.emplace_back(transform, light);
        else if (light.type == LightType::POINT)
            this->_pointLights.emplace_back(transform, light);
        else if (light.type == LightType::SPOT)
            this->_spotLights.emplace_back(transform, light);
    }

    this->_graphic->getRenderView().use("Model");
    for (auto &[id, entity] : entityManager.getEntities()) {
        if (!entity.hasComponent<Transform>() || !entity.hasComponent<Camera>())
            continue;
        auto &transform = entity.getComponent<Transform>();
        auto &camera = entity.getComponent<Camera>();

        this->_graphic->getRenderView().update(transform, camera);

        this->_graphic->getRenderView().setDirLights(this->_dirLights);
        this->_graphic->getRenderView().setPointLights(this->_pointLights);
    }
    this->_graphic->getRenderView().use("Map");
    this->_graphic->getRenderView().setDirLights(this->_dirLights);
    this->_graphic->getRenderView().setPointLights(this->_pointLights);

    this->_dirLights.clear();
    this->_pointLights.clear();
    this->_spotLights.clear();
}
