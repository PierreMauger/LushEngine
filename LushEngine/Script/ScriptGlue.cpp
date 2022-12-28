#include "ScriptGlue.hpp"

using namespace Lush;

void ScriptGlue::Transform_Get(std::size_t id, glm::vec3 *outTranslation)
{
    Engine *engine = Engine::getEngine();
    EntityManager &entityManager = engine->getEntityManager();
    ComponentManager &componentManager = engine->getComponentManager();

    if (entityManager.hasMask(id, ComponentType::TRANSFORM)) {
        Transform &transform = componentManager.getComponent<Transform>(id);
        *outTranslation = transform.position;
    } else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::Transform_Set(std::size_t id, glm::vec3 *translation)
{
    Engine *engine = Engine::getEngine();
    EntityManager &entityManager = engine->getEntityManager();
    ComponentManager &componentManager = engine->getComponentManager();

    if (entityManager.hasMask(id, ComponentType::TRANSFORM)) {
        Transform &transform = componentManager.getComponent<Transform>(id);
        transform.position.x = translation->x;
        transform.position.y = translation->y;
        transform.position.z = translation->z;
    } else
        std::cout << "Entity " << id << " has no Transform component" << std::endl;
}

void ScriptGlue::registerFunctions()
{
    mono_add_internal_call("InternalCalls::Transform_Get", (void *)Transform_Get);
    mono_add_internal_call("InternalCalls::Transform_Set", (void *)Transform_Set);
}
