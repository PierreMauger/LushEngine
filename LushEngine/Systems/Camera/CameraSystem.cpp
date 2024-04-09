#include "Systems/Camera/CameraSystem.hpp"

using namespace Lush;

CameraSystem::CameraSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
{
    Shapes::setupDepthBuffer(this->_shadowBuffer, {2048, 2048});
    this->_graphic->getFrameBuffers()["shadow"] = this->_shadowBuffer;
}

void CameraSystem::update(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
    for (auto &[id, entity] : entityManager->getEntities()) {
        if (entity.hasComponent<Transform>() && entity.hasComponent<Light>()) {
            Transform transform = entity.getComponent<Transform>();
            Light light = entity.getComponent<Light>();

            if (light.type == LightType::DIRECTIONAL) {
                this->drawShadowMap(entityManager, entity);
                this->_dirLights.emplace_back(transform, light);
            } else if (light.type == LightType::POINT)
                this->_pointLights.emplace_back(transform, light);
            else if (light.type == LightType::SPOT)
                this->_spotLights.emplace_back(transform, light);
        }
    }
    if (entityManager->hasEntity("MainCamera")) {
        Entity entity = entityManager->getEntity("MainCamera");
        if (entity.hasComponent<Transform>() && entity.hasComponent<Camera>())
            this->_graphic->getRenderView().update(entity.getComponent<Transform>(), entity.getComponent<Camera>());
    } else {
        for (auto &[id, entity] : entityManager->getEntities())
            if (entity.hasComponent<Transform>() && entity.hasComponent<Camera>())
                this->_graphic->getRenderView().update(entity.getComponent<Transform>(), entity.getComponent<Camera>());
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

void CameraSystem::drawShadowMap(std::shared_ptr<EntityManager> &entityManager, Entity lightEntity)
{
    Transform transform = lightEntity.getComponent<Transform>();
    Light light = lightEntity.getComponent<Light>();

    glViewport(0, 0, 2048, 2048);
    this->_graphic->getRenderView().use("Shadow");
    this->_graphic->getRenderView().setLightMatrix(transform, light);
    this->_graphic->getRenderView().setView();
    glBindFramebuffer(GL_FRAMEBUFFER, this->_shadowBuffer.depthbuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    for (auto &[id, entity] : entityManager->getEntities()) {
        if (entity.getParent().has_value() || !entity.hasComponent<Transform>() || !entity.hasComponent<Model>())
            continue;
        this->drawModels(entity, entityManager);
    }

    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, this->_graphic->getWindowSize().x, this->_graphic->getWindowSize().y);
}

void CameraSystem::drawModels(Entity &entity, std::shared_ptr<EntityManager> &entityManager, const Transform &parentTransform)
{
    Model model = entity.getComponent<Model>();
    Transform transform = entity.getComponent<Transform>();
    transform.rotation = parentTransform.rotation * transform.rotation;
    transform.position = parentTransform.rotation * transform.position + parentTransform.position;

    this->_graphic->getRenderView().setModel(transform);
    if (this->_resourceManager->getModels().contains(model.name))
        this->_resourceManager->getModels()[model.name]->draw(this->_graphic->getRenderView().getShader(), model, this->_resourceManager->getTextures());

    for (auto &childId : entity.getChildren()) {
        if (!entityManager->getEntities().contains(childId))
            continue;
        Entity &child = entityManager->getEntities()[childId];
        if (child.hasComponent<Transform>() && child.hasComponent<Model>())
            this->drawModels(child, entityManager, transform);
    }
}
