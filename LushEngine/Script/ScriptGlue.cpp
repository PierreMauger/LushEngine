#include "ScriptGlue.hpp"

using namespace Lush;

void ScriptGlue::Log(std::size_t id, MonoString *message, int type)
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

MonoString *ScriptGlue::GetName(std::size_t id)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return nullptr;

    return mono_string_new(mono_domain_get(), ECS::getStaticEntityManager()->getEntity(id).getName().c_str());
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

bool ScriptGlue::HasComponent(std::size_t id, MonoString *componentName)
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
        else
            hasComponent = entity.hasScriptComponent(utf8);
    }
    mono_free(utf8);
    return hasComponent;
}

void ScriptGlue::Transform_GetPosition(std::size_t id, glm::vec3 *position)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Transform>())
        *position = entity.getComponent<Transform>().position;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
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

void ScriptGlue::Transform_GetRotation(std::size_t id, glm::vec3 *rotation)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Transform>())
        *rotation = entity.getComponent<Transform>().rotation;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_SetRotation(std::size_t id, glm::vec3 *rotation)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Transform>())
        entity.getComponent<Transform>().rotation = *rotation;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_GetScale(std::size_t id, glm::vec3 *scale)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Transform>())
        *scale = entity.getComponent<Transform>().scale;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
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

void ScriptGlue::Camera_GetForward(std::size_t id, glm::vec3 *forward)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Camera>())
        *forward = entity.getComponent<Camera>().forward;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
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

void ScriptGlue::Collider_GetTag(std::size_t id, MonoString **tag)
{
    if (!ECS::getStaticEntityManager()->hasEntity(id))
        return;
    Entity &entity = ECS::getStaticEntityManager()->getEntity(id);

    if (entity.hasComponent<Collider>()) {
        *tag = mono_string_new(mono_domain_get(), entity.getComponent<Collider>().tag.c_str());
    } else
        std::cout << "[Toast Error]Entity " << id << " has no Collider component" << std::endl;
}

MonoObject *ScriptGlue::GetScriptInstance(std::size_t entityId, MonoString *scriptName)
{
    Entity &entity = ECS::getStaticEntityManager()->getEntity(entityId);
    std::string name = mono_string_to_utf8(scriptName);

    if (entity.hasScriptComponent(name))
        return ResourceManager::getStaticResourceManager()->getScriptInstances().at(entity.getScriptIndexes()[name]).getInstance();
    return nullptr;
}

bool ScriptGlue::IsKeyDown(int key)
{
    GLFWwindow *window = glfwGetCurrentContext();

    return window != nullptr && glfwGetKey(window, key) == GLFW_PRESS;
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

    if (resourceManager->getScenes().find(utf8) == resourceManager->getScenes().end()) {
        std::cout << "[Toast Error]Scene " << utf8 << " does not exist" << std::endl;
        return;
    }
    resourceManager->getScriptInstances().clear();
    resourceManager->resetDynamicsWorld();
    resourceManager->getPhysicInstances().clear();
    entityManager->clone(*resourceManager->getScenes()[utf8].getEntityManager());
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
    entityManager->clone(*resourceManager->getScenes()[resourceManager->getActiveScene()].getEntityManager());
    resourceManager->initScriptInstances(entityManager);
    resourceManager->initPhysicInstances(entityManager);
    resourceManager->setSceneChanged(true);
}

void ScriptGlue::DeleteEntity(std::size_t id)
{
    auto entityManager = ECS::getStaticEntityManager();

    if (!entityManager->hasEntity(id))
        return;
    entityManager->removeEntity(id);
}

void ScriptGlue::registerFunctions()
{
    mono_add_internal_call("InternalCalls::Log", (void *)Log);
    mono_add_internal_call("InternalCalls::GetName", (void *)GetName);
    mono_add_internal_call("InternalCalls::GetEntityFromName", (void *)GetEntityFromName);
    mono_add_internal_call("InternalCalls::HasComponent", (void *)HasComponent);
    mono_add_internal_call("InternalCalls::Transform_GetPosition", (void *)Transform_GetPosition);
    mono_add_internal_call("InternalCalls::Transform_SetPosition", (void *)Transform_SetPosition);
    mono_add_internal_call("InternalCalls::Transform_GetRotation", (void *)Transform_GetRotation);
    mono_add_internal_call("InternalCalls::Transform_SetRotation", (void *)Transform_SetRotation);
    mono_add_internal_call("InternalCalls::Transform_GetScale", (void *)Transform_GetScale);
    mono_add_internal_call("InternalCalls::Transform_SetScale", (void *)Transform_SetScale);
    mono_add_internal_call("InternalCalls::Camera_GetForward", (void *)Camera_GetForward);
    mono_add_internal_call("InternalCalls::Camera_SetForward", (void *)Camera_SetForward);
    mono_add_internal_call("InternalCalls::Collider_GetTag", (void *)Collider_GetTag);
    mono_add_internal_call("InternalCalls::GetScriptInstance", (void *)GetScriptInstance);
    mono_add_internal_call("InternalCalls::IsKeyDown", (void *)IsKeyDown);
    mono_add_internal_call("InternalCalls::GetMouseMovementX", (void *)GetMouseMovementX);
    mono_add_internal_call("InternalCalls::GetMouseMovementY", (void *)GetMouseMovementY);
    mono_add_internal_call("InternalCalls::SetMouseHidden", (void *)SetMouseHidden);
    mono_add_internal_call("InternalCalls::SetScene", (void *)SetScene);
    mono_add_internal_call("InternalCalls::ResetScene", (void *)ResetScene);
    mono_add_internal_call("InternalCalls::DeleteEntity", (void *)DeleteEntity);
}
