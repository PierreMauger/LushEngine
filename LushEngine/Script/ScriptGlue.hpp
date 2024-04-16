#ifndef SCRIPT_GLUE_HPP
#define SCRIPT_GLUE_HPP

#include "ECS/ECS.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "ResourceManager.hpp"
#include "mono/jit/jit.h"

namespace Lush
{
    class ScriptGlue
    {
        public:
            static MonoObject *Entity_GetScriptInstance(std::size_t entityId, MonoString *scriptName);
            static bool Entity_HasComponent(std::size_t id, MonoString *componentName);
            static void Entity_AddComponent(std::size_t id, MonoString *componentName);
            static void Entity_RemoveComponent(std::size_t id, MonoString *componentName);
            static void Entity_SetParent(std::size_t id, std::size_t parentId);
            static void Entity_Delete(std::size_t id);
            static MonoString *Entity_GetName(std::size_t id);
            static void Entity_Log(std::size_t id, MonoString *message, int type);

            static bool Transform_GetPosition(std::size_t id, glm::vec3 *position);
            static void Transform_SetPosition(std::size_t id, glm::vec3 *position);
            static bool Transform_GetRotation(std::size_t id, glm::quat *rotation);
            static void Transform_SetRotation(std::size_t id, glm::quat *rotation);
            static bool Transform_GetScale(std::size_t id, glm::vec3 *scale);
            static void Transform_SetScale(std::size_t id, glm::vec3 *scale);

            static bool Model_GetName(std::size_t id, MonoString **name);
            static void Model_SetName(std::size_t id, MonoString *name);
            static bool Model_GetMaterialColor(std::size_t id, MonoString *materialName, glm::vec3 *color);
            static void Model_SetMaterialColor(std::size_t id, MonoString *materialName, glm::vec3 *color);

            static bool Camera_GetForward(std::size_t id, glm::vec3 *forward);
            static void Camera_SetForward(std::size_t id, glm::vec3 *forward);
            static bool Camera_GetFOV(std::size_t id, float *fov);
            static void Camera_SetFOV(std::size_t id, float *fov);
            static bool Camera_GetNear(std::size_t id, float *near);
            static void Camera_SetNear(std::size_t id, float *near);
            static bool Camera_GetFar(std::size_t id, float *far);
            static void Camera_SetFar(std::size_t id, float *far);

            static bool Light_GetIntensity(std::size_t id, float *intensity);
            static void Light_SetIntensity(std::size_t id, float *intensity);
            static bool Light_GetColor(std::size_t id, glm::vec3 *color);
            static void Light_SetColor(std::size_t id, glm::vec3 *color);

            static bool Cubemap_GetName(std::size_t id, MonoString **name);
            static void Cubemap_SetName(std::size_t id, MonoString *name);

            static bool Billboard_GetName(std::size_t id, MonoString **name);
            static void Billboard_SetName(std::size_t id, MonoString *name);
            static bool Billboard_GetLockYAxis(std::size_t id, bool *lock);
            static void Billboard_SetLockYAxis(std::size_t id, bool *lock);

            static bool Collider_GetTag(std::size_t id, MonoString **tag);
            static void Collider_SetTag(std::size_t id, MonoString *tag);

            static bool IsKeyDown(int key);
            static unsigned long GetEntityFromName(MonoString *name);
            static float GetMouseMovementX();
            static float GetMouseMovementY();
            static void SetMouseHidden(bool state);
            static void SetScene(MonoString *sceneName);
            static void ResetScene();

            static void registerFunctions();
    };
}

#endif // SCRIPT_GLUE_HPP
