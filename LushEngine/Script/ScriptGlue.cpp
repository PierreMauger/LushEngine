#include "ScriptGlue.hpp"

using namespace Lush;

void ScriptGlue::Console_Log(std::size_t id, MonoString *message, int type)
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

void ScriptGlue::Transform_GetPosition(std::size_t id, glm::vec3 *position)
{
    Entity &entity = ECS::getECS()->getEntityManager().getEntity(id);

    if (entity.hasComponent<Transform>())
        *position = entity.getComponent<Transform>().position;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_SetPosition(std::size_t id, glm::vec3 *position)
{
    Entity &entity = ECS::getECS()->getEntityManager().getEntity(id);

    if (entity.hasComponent<Transform>())
        entity.getComponent<Transform>().position = *position;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_GetRotation(std::size_t id, glm::vec3 *rotation)
{
    Entity &entity = ECS::getECS()->getEntityManager().getEntity(id);

    if (entity.hasComponent<Transform>())
        *rotation = entity.getComponent<Transform>().rotation;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_SetRotation(std::size_t id, glm::vec3 *rotation)
{
    Entity &entity = ECS::getECS()->getEntityManager().getEntity(id);

    if (entity.hasComponent<Transform>())
        entity.getComponent<Transform>().rotation = *rotation;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_GetScale(std::size_t id, glm::vec3 *scale)
{
    Entity &entity = ECS::getECS()->getEntityManager().getEntity(id);

    if (entity.hasComponent<Transform>())
        *scale = entity.getComponent<Transform>().scale;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_SetScale(std::size_t id, glm::vec3 *scale)
{
    Entity &entity = ECS::getECS()->getEntityManager().getEntity(id);

    if (entity.hasComponent<Transform>())
        entity.getComponent<Transform>().scale = *scale;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Camera_GetForward(std::size_t id, glm::vec3 *forward)
{
    Entity &entity = ECS::getECS()->getEntityManager().getEntity(id);

    if (entity.hasComponent<Camera>())
        *forward = entity.getComponent<Camera>().forward;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
}

void ScriptGlue::Camera_SetForward(std::size_t id, glm::vec3 *forward)
{
    Entity &entity = ECS::getECS()->getEntityManager().getEntity(id);

    if (entity.hasComponent<Camera>())
        entity.getComponent<Camera>().forward = *forward;
    else
        std::cout << "[Toast Error]Entity " << id << " has no Camera component" << std::endl;
}

static std::size_t getScriptInstanceIndex(ResourceManager *resourceManager, std::size_t entityId)
{
    std::size_t i = 0;

    for (auto &instance : resourceManager->getInstances()) {
        if (instance.getId() == entityId)
            return i;
        i++;
    }
    return (std::size_t)-1;
}

MonoObject *ScriptGlue::GetScriptInstance(std::size_t id)
{
    ResourceManager *resourceManager = ResourceManager::getResourceManager();
    std::size_t index = getScriptInstanceIndex(resourceManager, id);
    if (index != (std::size_t)-1)
        return resourceManager->getInstances().at(index).getInstance();
    else
        return nullptr;
}

bool ScriptGlue::IsKeyDown(int key)
{
    GLFWwindow *window = glfwGetCurrentContext();

    return window != nullptr && glfwGetKey(window, key) == GLFW_PRESS;
}

float ScriptGlue::GetMouseMovementX()
{
    return Graphic::getGraphic()->getMouseOffset().x;
}

float ScriptGlue::GetMouseMovementY()
{
    return Graphic::getGraphic()->getMouseOffset().y;
}

void ScriptGlue::registerFunctions()
{
    mono_add_internal_call("InternalCalls::Log", (void *)Console_Log);
    mono_add_internal_call("InternalCalls::Transform_GetPosition", (void *)Transform_GetPosition);
    mono_add_internal_call("InternalCalls::Transform_SetPosition", (void *)Transform_SetPosition);
    mono_add_internal_call("InternalCalls::Transform_GetRotation", (void *)Transform_GetRotation);
    mono_add_internal_call("InternalCalls::Transform_SetRotation", (void *)Transform_SetRotation);
    mono_add_internal_call("InternalCalls::Transform_GetScale", (void *)Transform_GetScale);
    mono_add_internal_call("InternalCalls::Transform_SetScale", (void *)Transform_SetScale);
    mono_add_internal_call("InternalCalls::Camera_GetForward", (void *)Camera_GetForward);
    mono_add_internal_call("InternalCalls::Camera_SetForward", (void *)Camera_SetForward);
    mono_add_internal_call("InternalCalls::GetScriptInstance", (void *)GetScriptInstance);
    mono_add_internal_call("InternalCalls::IsKeyDown", (void *)IsKeyDown);
    mono_add_internal_call("InternalCalls::GetMouseMovementX", (void *)GetMouseMovementX);
    mono_add_internal_call("InternalCalls::GetMouseMovementY", (void *)GetMouseMovementY);
}
