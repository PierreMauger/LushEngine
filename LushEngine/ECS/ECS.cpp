#include "ECS/ECS.hpp"

using namespace Lush;

static ECS *ecs = nullptr;

ECS *ECS::getECS()
{
    return ecs;
}

ECS::ECS()
{
    ecs = this;
}

void ECS::loadComponents()
{
    this->_componentManager.bindComponent<Transform>();
    this->_componentManager.bindComponent<Velocity>();
    this->_componentManager.bindComponent<Model>();
    this->_componentManager.bindComponent<Camera>();
    this->_componentManager.bindComponent<Light>();
    this->_componentManager.bindComponent<CubeMap>();
    this->_componentManager.bindComponent<BillBoard>();
    this->_componentManager.bindComponent<Map>();
}

void ECS::loadSystems(std::shared_ptr<Graphic> graphic)
{
    this->_systemManager.bindSystem(std::make_unique<ScriptSystem>(graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<CameraSystem>(graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<RenderSystem>(graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<SceneSystem>(graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<PickingSystem>(graphic, this->_entityManager));
    this->_systemManager.bindSystem(std::make_unique<GUISystem>(graphic));
}

EntityManager &ECS::getEntityManager()
{
    return this->_entityManager;
}

ComponentManager &ECS::getComponentManager()
{
    return this->_componentManager;
}

SystemManager &ECS::getSystemManager()
{
    return this->_systemManager;
}
