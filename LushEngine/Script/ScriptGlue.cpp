#include "ScriptGlue.hpp"

using namespace Lush;

void ScriptGlue::Console_Log(std::size_t id, MonoString *message)
{
    char *utf8 = mono_string_to_utf8(message);
    std::cout << "[" << id << "]: " << utf8 << std::endl;
    mono_free(utf8);
}

void ScriptGlue::Transform_GetPosition(std::size_t id, glm::vec3 *position)
{
    ECS *ecs = ECS::getECS();

    if (ecs->getEntityManager().hasMask(id, ComponentType::TRANSFORM))
        *position = ecs->getComponentManager().getComponent<Transform>(id).position;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_SetPosition(std::size_t id, glm::vec3 *position)
{
    ECS *ecs = ECS::getECS();

    if (ecs->getEntityManager().hasMask(id, ComponentType::TRANSFORM))
        ecs->getComponentManager().getComponent<Transform>(id).position = *position;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_GetRotation(std::size_t id, glm::vec3 *rotation)
{
    ECS *ecs = ECS::getECS();

    if (ecs->getEntityManager().hasMask(id, ComponentType::TRANSFORM))
        *rotation = ecs->getComponentManager().getComponent<Transform>(id).rotation;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_SetRotation(std::size_t id, glm::vec3 *rotation)
{
    ECS *ecs = ECS::getECS();

    if (ecs->getEntityManager().hasMask(id, ComponentType::TRANSFORM))
        ecs->getComponentManager().getComponent<Transform>(id).rotation = *rotation;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_GetScale(std::size_t id, glm::vec3 *scale)
{
    ECS *ecs = ECS::getECS();

    if (ecs->getEntityManager().hasMask(id, ComponentType::TRANSFORM))
        *scale = ecs->getComponentManager().getComponent<Transform>(id).scale;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_SetScale(std::size_t id, glm::vec3 *scale)
{
    ECS *ecs = ECS::getECS();

    if (ecs->getEntityManager().hasMask(id, ComponentType::TRANSFORM))
        ecs->getComponentManager().getComponent<Transform>(id).scale = *scale;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Camera_GetForward(std::size_t id, glm::vec3 *forward)
{
    ECS *ecs = ECS::getECS();

    if (ecs->getEntityManager().hasMask(id, ComponentType::CAMERA))
        *forward = ecs->getComponentManager().getComponent<Camera>(id).forward;
    else
        std::cout << "Entity " << id << " has no Camera component" << std::endl;
}

void ScriptGlue::Camera_SetForward(std::size_t id, glm::vec3 *forward)
{
    ECS *ecs = ECS::getECS();

    if (ecs->getEntityManager().hasMask(id, ComponentType::CAMERA))
        ecs->getComponentManager().getComponent<Camera>(id).forward = *forward;
    else
        std::cout << "Entity " << id << " has no Camera component" << std::endl;
}

bool ScriptGlue::IsKeyDown(int key)
{
    GLFWwindow *window = glfwGetCurrentContext();

    if (window)
        return glfwGetKey(window, key) == GLFW_PRESS;
    else
        return false;
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
    mono_add_internal_call("InternalCalls::IsKeyDown", (void *)IsKeyDown);
}
