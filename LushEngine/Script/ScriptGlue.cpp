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
    Engine *engine = Engine::getEngine();
    EntityManager &entityManager = engine->getEntityManager();
    ComponentManager &componentManager = engine->getComponentManager();

    if (entityManager.hasMask(id, ComponentType::TRANSFORM))
        *position = componentManager.getComponent<Transform>(id).position;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_SetPosition(std::size_t id, glm::vec3 *position)
{
    Engine *engine = Engine::getEngine();
    EntityManager &entityManager = engine->getEntityManager();
    ComponentManager &componentManager = engine->getComponentManager();

    if (entityManager.hasMask(id, ComponentType::TRANSFORM))
        componentManager.getComponent<Transform>(id).position = *position;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_GetRotation(std::size_t id, glm::vec3 *rotation)
{
    Engine *engine = Engine::getEngine();
    EntityManager &entityManager = engine->getEntityManager();
    ComponentManager &componentManager = engine->getComponentManager();

    if (entityManager.hasMask(id, ComponentType::TRANSFORM))
        *rotation = componentManager.getComponent<Transform>(id).rotation;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_SetRotation(std::size_t id, glm::vec3 *rotation)
{
    Engine *engine = Engine::getEngine();
    EntityManager &entityManager = engine->getEntityManager();
    ComponentManager &componentManager = engine->getComponentManager();

    if (entityManager.hasMask(id, ComponentType::TRANSFORM))
        componentManager.getComponent<Transform>(id).rotation = *rotation;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_GetScale(std::size_t id, glm::vec3 *scale)
{
    Engine *engine = Engine::getEngine();
    EntityManager &entityManager = engine->getEntityManager();
    ComponentManager &componentManager = engine->getComponentManager();

    if (entityManager.hasMask(id, ComponentType::TRANSFORM))
        *scale = componentManager.getComponent<Transform>(id).scale;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_SetScale(std::size_t id, glm::vec3 *scale)
{
    Engine *engine = Engine::getEngine();
    EntityManager &entityManager = engine->getEntityManager();
    ComponentManager &componentManager = engine->getComponentManager();

    if (entityManager.hasMask(id, ComponentType::TRANSFORM))
        componentManager.getComponent<Transform>(id).scale = *scale;
    else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
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
}
