#include "ScriptGlue.hpp"

using namespace Lush;

MonoObject *ScriptGlue::Entity_GetScriptInstance(std::size_t entityId, MonoString *scriptName)
{
    Entity &entity = ECS::getStaticEntityManager()->getEntity(entityId);
    std::string name = mono_string_to_utf8(scriptName);

    if (entity.hasScriptComponent(name))
        return ResourceManager::getStaticResourceManager()->getScriptInstances().at(entity.getScriptIndexes()[name]).getInstance();
    return nullptr;
}

bool ScriptGlue::Entity_HasComponent(std::size_t id, MonoString *componentName)
{
    char *utf8 = mono_string_to_utf8(componentName);
    bool hasComponent = false;
    if (ECS::getStaticEntityManager()->hasEntity(id)) {
        Entity &entity = ECS::getStaticEntityManager()->getEntity(id);
        if (std::string(utf8) == "Transform")
            hasComponent = entity.hasComponent<Transform>();
        else if (std::string(utf8) == "Model")
            hasComponent = entity.hasComponent<Model>();
        else if (std::string(utf8) == "Camera")
            hasComponent = entity.hasComponent<Camera>();
        else if (std::string(utf8) == "Light")
            hasComponent = entity.hasComponent<Light>();
        else if (std::string(utf8) == "Cubemap")
            hasComponent = entity.hasComponent<Cubemap>();
        else if (std::string(utf8) == "Billboard")
            hasComponent = entity.hasComponent<Billboard>();
        else if (std::string(utf8) == "Map")
            hasComponent = entity.hasComponent<Map>();
        else if (std::string(utf8) == "RigidBody")
            hasComponent = entity.hasComponent<RigidBody>();
        else if (std::string(utf8) == "Collider")
            hasComponent = entity.hasComponent<Collider>();
        else if (std::string(utf8) == "CharacterController")
            hasComponent = entity.hasComponent<CharacterController>();
        else if (std::string(utf8) == "UIElement")
            hasComponent = entity.hasComponent<UIElement>();
        else
            hasComponent = entity.hasScriptComponent(utf8);
    }
    mono_free(utf8);
    return hasComponent;
}

void ScriptGlue::Entity_AddComponent(std::size_t id, MonoString *componentName)
{
    char *utf8 = mono_string_to_utf8(componentName);
    if (ECS::getStaticEntityManager()->hasEntity(id)) {
        Entity &entity = ECS::getStaticEntityManager()->getEntity(id);
        if (std::string(utf8) == "Transform")
            entity.addComponent<Transform>(Transform());
        else if (std::string(utf8) == "Model")
            entity.addComponent<Model>(Model());
        else if (std::string(utf8) == "Camera")
            entity.addComponent<Camera>(Camera());
        else if (std::string(utf8) == "Light")
            entity.addComponent<Light>(Light());
        else if (std::string(utf8) == "Cubemap")
            entity.addComponent<Cubemap>(Cubemap());
        else if (std::string(utf8) == "Billboard")
            entity.addComponent<Billboard>(Billboard());
        else if (std::string(utf8) == "Map")
            entity.addComponent<Map>(Map());
        else if (std::string(utf8) == "RigidBody") {
            entity.addComponent<RigidBody>(RigidBody());
            ResourceManager::getStaticResourceManager()->initPhysicInstance(id, entity);
        } else if (std::string(utf8) == "Collider")
            entity.addComponent<Collider>(Collider());
        else if (std::string(utf8) == "CharacterController")
            entity.addComponent<CharacterController>(CharacterController());
        else if (std::string(utf8) == "UIElement")
            entity.addComponent<UIElement>(UIElement());
        else {
            auto resourceManager = ResourceManager::getStaticResourceManager();
            if (resourceManager->getScripts().contains(utf8)) {
                ScriptComponent scriptComponent(resourceManager->getScripts()[utf8]);
                entity.addScriptComponent(std::string(utf8), scriptComponent);
                std::size_t scriptId = 0;
                if (!resourceManager->getScriptInstances().empty())
                    scriptId = resourceManager->getScriptInstances().rbegin()->first + 1;
                entity.addScriptIndex(utf8, scriptId);
                resourceManager->getScriptInstances()[scriptId] = ScriptInstance(resourceManager->getScripts()[utf8], id, scriptComponent.getFields());
            }
        }
    }
    mono_free(utf8);
}

void ScriptGlue::Entity_RemoveComponent(std::size_t id, MonoString *componentName)
{
    char *utf8 = mono_string_to_utf8(componentName);
    if (ECS::getStaticEntityManager()->hasEntity(id)) {
        Entity &entity = ECS::getStaticEntityManager()->getEntity(id);
        if (std::string(utf8) == "Transform")
            entity.removeComponent<Transform>();
        else if (std::string(utf8) == "Model")
            entity.removeComponent<Model>();
        else if (std::string(utf8) == "Camera")
            entity.removeComponent<Camera>();
        else if (std::string(utf8) == "Light")
            entity.removeComponent<Light>();
        else if (std::string(utf8) == "Cubemap")
            entity.removeComponent<Cubemap>();
        else if (std::string(utf8) == "Billboard")
            entity.removeComponent<Billboard>();
        else if (std::string(utf8) == "Map")
            entity.removeComponent<Map>();
        else if (std::string(utf8) == "RigidBody")
            entity.removeComponent<RigidBody>();
        else if (std::string(utf8) == "Collider") {
            entity.removeComponent<Collider>();
            auto &instances = ResourceManager::getStaticResourceManager()->getPhysicInstances();
            auto instance = std::find_if(instances.begin(), instances.end(), [id](const auto &instance) { return instance->getId() == id; });
            if (instance == instances.end())
                return;
            (*instance)->removeFromWorld(ResourceManager::getStaticResourceManager()->getDynamicsWorld());
            instances.erase(instance);
        } else if (std::string(utf8) == "CharacterController")
            entity.removeComponent<CharacterController>();
        else if (std::string(utf8) == "UIElement")
            entity.removeComponent<UIElement>();
        else {
            ResourceManager::getStaticResourceManager()->getScriptInstances().erase(entity.getScriptIndexes()[std::string(utf8)]);
            entity.removeScriptComponent(std::string(utf8));
        }
    }
    mono_free(utf8);
}

void ScriptGlue::Entity_SetParent(std::size_t id, std::size_t parentId)
{
    auto entityManager = ECS::getStaticEntityManager();
    Entity &entity = entityManager->getEntity(id);
    Entity &parent = entityManager->getEntity(parentId);

    if (!entityManager->hasEntity(id) || !entityManager->hasEntity(parentId) || id == parentId)
        return;
    entity.setParent(parentId);
    parent.addChild(id);

    if (entity.hasComponent<Transform>() && parent.hasComponent<Transform>()) {
        Transform &transform = entity.getComponent<Transform>();
        Transform &parentTransform = parent.getComponent<Transform>();
        transform.rotation = parentTransform.rotation * transform.rotation;
        transform.position = parentTransform.rotation * transform.position + parentTransform.position;
    }
}

void ScriptGlue::Entity_RemoveParent(std::size_t id)
{
    auto entityManager = ECS::getStaticEntityManager();
    Entity &entity = entityManager->getEntity(id);
    if (!entity.getParent().has_value())
        return;
    Entity &parent = entityManager->getEntity(entity.getParent().value());

    entity.removeParent();
    parent.removeChild(id);

    if (entity.hasComponent<Transform>() && parent.hasComponent<Transform>()) {
        Transform &transform = entity.getComponent<Transform>();
        Transform &parentTransform = parent.getComponent<Transform>();
        transform.rotation = glm::inverse(parentTransform.rotation) * transform.rotation;
        transform.position = glm::inverse(parentTransform.rotation) * (transform.position - parentTransform.position);
    }
}

MonoString *ScriptGlue::Entity_GetName(std::size_t id)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return nullptr;

    return mono_string_new(mono_domain_get(), ECS::getStaticEntityManager()->getEntity(id).getName().c_str());
}

void ScriptGlue::Entity_Delete(std::size_t id)
{
    auto entityManager = ECS::getStaticEntityManager();

    if (!entityManager->hasEntity(id))
        return;
    entityManager->removeEntity(id);
}

void ScriptGlue::Entity_Log(std::size_t id, MonoString *message, int type)
{
    char *utf8 = mono_string_to_utf8(message);
    switch (type) {
    case 1:
        std::cout << "[Toast Success]";
        break;
    case 2:
        std::cout << "[Toast Info]";
        break;
    case 3:
        std::cout << "[Toast Warning]";
        break;
    case 4:
        std::cout << "[Toast Error]";
        break;
    default:
        break;
    }
    std::cout << "[" << id << "]: " << utf8 << std::endl;
    mono_free(utf8);
}

bool ScriptGlue::Transform_GetPosition(std::size_t id, glm::vec3 *position)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Transform>()) {
        *position = entity.getComponent<Transform>().position;
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
    return false;
}

void ScriptGlue::Transform_SetPosition(std::size_t id, glm::vec3 *position)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Transform>())
        entity.getComponent<Transform>().position = *position;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

bool ScriptGlue::Transform_GetRotation(std::size_t id, glm::quat *rotation)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Transform>()) {
        *rotation = entity.getComponent<Transform>().rotation;
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
    return false;
}

void ScriptGlue::Transform_SetRotation(std::size_t id, glm::quat *rotation)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Transform>())
        entity.getComponent<Transform>().rotation = *rotation;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

bool ScriptGlue::Transform_GetScale(std::size_t id, glm::vec3 *scale)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Transform>()) {
        *scale = entity.getComponent<Transform>().scale;
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
    return false;
}

void ScriptGlue::Transform_SetScale(std::size_t id, glm::vec3 *scale)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Transform>())
        entity.getComponent<Transform>().scale = *scale;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

bool ScriptGlue::Model_GetName(std::size_t id, MonoString **name)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Model>()) {
        *name = mono_string_new(mono_domain_get(), entity.getComponent<Model>().name.c_str());
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Model component" << std::endl;
    return false;
}

void ScriptGlue::Model_SetName(std::size_t id, MonoString *name)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);
    char *utf8 = mono_string_to_utf8(name);

    if (entity.hasComponent<Model>())
        entity.getComponent<Model>().name = utf8;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Model component" << std::endl;
    mono_free(utf8);
}

bool ScriptGlue::Model_GetMaterialColor(std::size_t id, MonoString *materialName, glm::vec3 *color)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);
    char *utf8 = mono_string_to_utf8(materialName);

    if (entity.hasComponent<Model>()) {
        if (entity.getComponent<Model>().materials.contains(utf8)) {
            *color = entity.getComponent<Model>().materials[utf8].diffuse;
            mono_free(utf8);
            return true;
        }
    }
    std::cout << "[Toast Error]Entity " << id << " has no Model component" << std::endl;
    mono_free(utf8);
    return false;
}

void ScriptGlue::Model_SetMaterialColor(std::size_t id, MonoString *materialName, glm::vec3 *color)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);
    char *utf8 = mono_string_to_utf8(materialName);

    if (entity.hasComponent<Model>()) {
        if (entity.getComponent<Model>().materials.contains(utf8))
            entity.getComponent<Model>().materials[utf8].diffuse = *color;
    } else
        std::cout << "[Toast Error]Entity " << id << " has no Model component" << std::endl;
    mono_free(utf8);
}

bool ScriptGlue::Camera_GetForward(std::size_t id, glm::vec3 *forward)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Camera>()) {
        *forward = entity.getComponent<Camera>().forward;
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
    return false;
}

void ScriptGlue::Camera_SetForward(std::size_t id, glm::vec3 *forward)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Camera>())
        entity.getComponent<Camera>().forward = *forward;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
}

bool ScriptGlue::Camera_GetFOV(std::size_t id, float *fov)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Camera>()) {
        *fov = entity.getComponent<Camera>().fov;
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
    return false;
}

void ScriptGlue::Camera_SetFOV(std::size_t id, const float *fov)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Camera>())
        entity.getComponent<Camera>().fov = *fov;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
}

bool ScriptGlue::Camera_GetNear(std::size_t id, float *near)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Camera>()) {
        *near = entity.getComponent<Camera>().near;
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
    return false;
}

void ScriptGlue::Camera_SetNear(std::size_t id, const float *near)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Camera>())
        entity.getComponent<Camera>().near = *near;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
}

bool ScriptGlue::Camera_GetFar(std::size_t id, float *far)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Camera>()) {
        *far = entity.getComponent<Camera>().far;
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
    return false;
}

void ScriptGlue::Camera_SetFar(std::size_t id, const float *far)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Camera>())
        entity.getComponent<Camera>().far = *far;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
}

bool ScriptGlue::Light_GetIntensity(std::size_t id, float *intensity)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Light>()) {
        *intensity = entity.getComponent<Light>().intensity;
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Light component" << std::endl;
    return false;
}

void ScriptGlue::Light_SetIntensity(std::size_t id, const float *intensity)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Light>())
        entity.getComponent<Light>().intensity = *intensity;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Light component" << std::endl;
}

bool ScriptGlue::Light_GetColor(std::size_t id, glm::vec3 *color)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Light>()) {
        *color = entity.getComponent<Light>().color;
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Light component" << std::endl;
    return false;
}

void ScriptGlue::Light_SetColor(std::size_t id, glm::vec3 *color)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Light>())
        entity.getComponent<Light>().color = *color;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Light component" << std::endl;
}

bool ScriptGlue::Cubemap_GetName(std::size_t id, MonoString **name)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Cubemap>()) {
        *name = mono_string_new(mono_domain_get(), entity.getComponent<Cubemap>().name.c_str());
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Cubemap component" << std::endl;
    return false;
}

void ScriptGlue::Cubemap_SetName(std::size_t id, MonoString *name)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);
    char *utf8 = mono_string_to_utf8(name);

    if (entity.hasComponent<Cubemap>())
        entity.getComponent<Cubemap>().name = utf8;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Cubemap component" << std::endl;
    mono_free(utf8);
}

bool ScriptGlue::Billboard_GetName(std::size_t id, MonoString **name)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Billboard>()) {
        *name = mono_string_new(mono_domain_get(), entity.getComponent<Billboard>().name.c_str());
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Billboard component" << std::endl;
    return false;
}

void ScriptGlue::Billboard_SetName(std::size_t id, MonoString *name)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);
    char *utf8 = mono_string_to_utf8(name);

    if (entity.hasComponent<Billboard>())
        entity.getComponent<Billboard>().name = utf8;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Billboard component" << std::endl;
    mono_free(utf8);
}

bool ScriptGlue::Billboard_GetLockYAxis(std::size_t id, bool *lock)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Billboard>()) {
        *lock = entity.getComponent<Billboard>().lockYAxis;
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Billboard component" << std::endl;
    return false;
}

void ScriptGlue::Billboard_SetLockYAxis(std::size_t id, const bool *lock)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Billboard>())
        entity.getComponent<Billboard>().lockYAxis = *lock;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Billboard component" << std::endl;
}

bool ScriptGlue::Collider_GetTag(std::size_t id, MonoString **tag)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Collider>()) {
        *tag = mono_string_new(mono_domain_get(), entity.getComponent<Collider>().tag.c_str());
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no Collider component" << std::endl;
    return false;
}

void ScriptGlue::Collider_SetTag(std::size_t id, MonoString *tag)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Collider>()) {
        char *utf8 = mono_string_to_utf8(tag);
        entity.getComponent<Collider>().tag = utf8;
        mono_free(utf8);
    } else
        std::cout << "[Toast Error]Entity " << id << " has no Collider component" << std::endl;
}

bool ScriptGlue::RigidBody_GetVelocity(std::size_t id, glm::vec3 *velocity)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return false;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);
    auto &instances = ResourceManager::getStaticResourceManager()->getPhysicInstances();
    auto instance = std::find_if(instances.begin(), instances.end(), [id](const auto &instance) { return instance->getId() == id; });

    if (entity.hasComponent<RigidBody>() && instance != instances.end()) {
        *velocity = (*instance)->getVelocity();
        return true;
    }
    std::cout << "[Toast Error]Entity " << id << " has no RigidBody component" << std::endl;
    return false;
}

void ScriptGlue::RigidBody_SetVelocity(std::size_t id, glm::vec3 *velocity)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);
    auto &instances = ResourceManager::getStaticResourceManager()->getPhysicInstances();
    auto instance = std::find_if(instances.begin(), instances.end(), [id](const auto &instance) { return instance->getId() == id; });

    if (entity.hasComponent<RigidBody>() && instance != instances.end())
        (*instance)->setVelocity(*velocity);
    else
        std::cout << "[Toast Error]Entity " << id << " has no RigidBody component" << std::endl;
}

void ScriptGlue::RigidBody_AddForce(std::size_t id, glm::vec3 *force)
{
    auto &instances = ResourceManager::getStaticResourceManager()->getPhysicInstances();
    auto instance = std::find_if(instances.begin(), instances.end(), [id](const auto &instance) { return instance->getId() == id; });

    if (instance == instances.end())
        return;
    (*instance)->addForce(*force);
}

bool ScriptGlue::IsKeyDown(int key)
{
    GLFWwindow *window = glfwGetCurrentContext();

    return window != nullptr && glfwGetKey(window, key) == GLFW_PRESS;
}

unsigned long ScriptGlue::GetEntityFromName(MonoString *name)
{
    char *utf8 = mono_string_to_utf8(name);
    unsigned long id = 0;

    if (ECS::getStaticEntityManager()->hasEntity(utf8))
        id = ECS::getStaticEntityManager()->getEntityIndex(utf8);

    mono_free(utf8);
    return id;
}

float ScriptGlue::GetMouseMovementX()
{
    auto graphic = Graphic::getGraphic();

    if (graphic->isRunning() && !graphic->isMouseHidden())
        return 0;
    return graphic->getMouseOffset().x;
}

float ScriptGlue::GetMouseMovementY()
{
    auto graphic = Graphic::getGraphic();

    if (graphic->isRunning() && !graphic->isMouseHidden())
        return 0;
    return graphic->getMouseOffset().y;
}

void ScriptGlue::SetMouseHidden(bool state)
{
    Graphic::getGraphic()->setMouseHidden(state);
}

void ScriptGlue::SetScene(MonoString *sceneName)
{
    auto resourceManager = ResourceManager::getStaticResourceManager();
    auto entityManager = ECS::getStaticEntityManager();
    char *utf8 = mono_string_to_utf8(sceneName);

    if (!resourceManager->getScenes().contains(utf8)) {
        std::cout << "[Toast Error]Scene " << utf8 << " does not exist" << std::endl;
        return;
    }
    resourceManager->getScriptInstances().clear();
    resourceManager->resetDynamicsWorld();
    resourceManager->getPhysicInstances().clear();
    entityManager->clone(*resourceManager->getScenes()[utf8]->getEntityManager());
    resourceManager->setActiveScene(utf8);
    resourceManager->initScriptInstances(entityManager);
    resourceManager->initPhysicInstances(entityManager);
    resourceManager->setSceneChanged(true);
    mono_free(utf8);
}

void ScriptGlue::ResetScene()
{
    auto resourceManager = ResourceManager::getStaticResourceManager();
    auto entityManager = ECS::getStaticEntityManager();

    resourceManager->getScriptInstances().clear();
    resourceManager->resetDynamicsWorld();
    resourceManager->getPhysicInstances().clear();
    entityManager->clone(*resourceManager->getActiveScene().getEntityManager());
    resourceManager->initScriptInstances(entityManager);
    resourceManager->initPhysicInstances(entityManager);
    resourceManager->setSceneChanged(true);
}

void ScriptGlue::registerFunctions()
{
    mono_add_internal_call("InternalCalls::Entity_GetScriptInstance", (void *)Entity_GetScriptInstance);
    mono_add_internal_call("InternalCalls::Entity_HasComponent", (void *)Entity_HasComponent);
    mono_add_internal_call("InternalCalls::Entity_AddComponent", (void *)Entity_AddComponent);
    mono_add_internal_call("InternalCalls::Entity_RemoveComponent", (void *)Entity_RemoveComponent);
    mono_add_internal_call("InternalCalls::Entity_SetParent", (void *)Entity_SetParent);
    mono_add_internal_call("InternalCalls::Entity_RemoveParent", (void *)Entity_RemoveParent);
    mono_add_internal_call("InternalCalls::Entity_GetName", (void *)Entity_GetName);
    mono_add_internal_call("InternalCalls::Entity_Delete", (void *)Entity_Delete);
    mono_add_internal_call("InternalCalls::Entity_Log", (void *)Entity_Log);
    mono_add_internal_call("InternalCalls::Transform_GetPosition", (void *)Transform_GetPosition);
    mono_add_internal_call("InternalCalls::Transform_SetPosition", (void *)Transform_SetPosition);
    mono_add_internal_call("InternalCalls::Transform_GetRotation", (void *)Transform_GetRotation);
    mono_add_internal_call("InternalCalls::Transform_SetRotation", (void *)Transform_SetRotation);
    mono_add_internal_call("InternalCalls::Transform_GetScale", (void *)Transform_GetScale);
    mono_add_internal_call("InternalCalls::Transform_SetScale", (void *)Transform_SetScale);
    mono_add_internal_call("InternalCalls::Model_GetName", (void *)Model_GetName);
    mono_add_internal_call("InternalCalls::Model_SetName", (void *)Model_SetName);
    mono_add_internal_call("InternalCalls::Model_GetMaterialColor", (void *)Model_GetMaterialColor);
    mono_add_internal_call("InternalCalls::Model_SetMaterialColor", (void *)Model_SetMaterialColor);
    mono_add_internal_call("InternalCalls::Camera_GetForward", (void *)Camera_GetForward);
    mono_add_internal_call("InternalCalls::Camera_SetForward", (void *)Camera_SetForward);
    mono_add_internal_call("InternalCalls::Camera_GetFOV", (void *)Camera_GetFOV);
    mono_add_internal_call("InternalCalls::Camera_SetFOV", (void *)Camera_SetFOV);
    mono_add_internal_call("InternalCalls::Camera_GetNear", (void *)Camera_GetNear);
    mono_add_internal_call("InternalCalls::Camera_SetNear", (void *)Camera_SetNear);
    mono_add_internal_call("InternalCalls::Camera_GetFar", (void *)Camera_GetFar);
    mono_add_internal_call("InternalCalls::Camera_SetFar", (void *)Camera_SetFar);
    mono_add_internal_call("InternalCalls::Light_GetIntensity", (void *)Light_GetIntensity);
    mono_add_internal_call("InternalCalls::Light_SetIntensity", (void *)Light_SetIntensity);
    mono_add_internal_call("InternalCalls::Light_GetColor", (void *)Light_GetColor);
    mono_add_internal_call("InternalCalls::Light_SetColor", (void *)Light_SetColor);
    mono_add_internal_call("InternalCalls::Cubemap_GetName", (void *)Cubemap_GetName);
    mono_add_internal_call("InternalCalls::Cubemap_SetName", (void *)Cubemap_SetName);
    mono_add_internal_call("InternalCalls::Billboard_GetName", (void *)Billboard_GetName);
    mono_add_internal_call("InternalCalls::Billboard_SetName", (void *)Billboard_SetName);
    mono_add_internal_call("InternalCalls::Billboard_GetLockYAxis", (void *)Billboard_GetLockYAxis);
    mono_add_internal_call("InternalCalls::Billboard_SetLockYAxis", (void *)Billboard_SetLockYAxis);
    mono_add_internal_call("InternalCalls::Collider_GetTag", (void *)Collider_GetTag);
    mono_add_internal_call("InternalCalls::Collider_SetTag", (void *)Collider_SetTag);
    mono_add_internal_call("InternalCalls::RigidBody_GetVelocity", (void *)RigidBody_GetVelocity);
    mono_add_internal_call("InternalCalls::RigidBody_SetVelocity", (void *)RigidBody_SetVelocity);
    mono_add_internal_call("InternalCalls::RigidBody_AddForce", (void *)RigidBody_AddForce);
    mono_add_internal_call("InternalCalls::IsKeyDown", (void *)IsKeyDown);
    mono_add_internal_call("InternalCalls::GetEntityFromName", (void *)GetEntityFromName);
    mono_add_internal_call("InternalCalls::GetMouseMovementX", (void *)GetMouseMovementX);
    mono_add_internal_call("InternalCalls::GetMouseMovementY", (void *)GetMouseMovementY);
    mono_add_internal_call("InternalCalls::SetMouseHidden", (void *)SetMouseHidden);
    mono_add_internal_call("InternalCalls::SetScene", (void *)SetScene);
    mono_add_internal_call("InternalCalls::ResetScene", (void *)ResetScene);
}
