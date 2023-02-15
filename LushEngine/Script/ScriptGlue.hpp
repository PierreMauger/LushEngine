#ifndef SCRIPTGLUE_HPP
#define SCRIPTGLUE_HPP

#include "ECS/ECS.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "mono/jit/jit.h"

namespace Lush
{
    class ScriptGlue
    {
        public:
            static void Console_Log(std::size_t id, MonoString *message);
            // static std::size_t Entity_FindEntity(std::size_t id);

            static void Transform_GetPosition(std::size_t id, glm::vec3 *position);
            static void Transform_SetPosition(std::size_t id, glm::vec3 *position);
            static void Transform_GetRotation(std::size_t id, glm::vec3 *rotation);
            static void Transform_SetRotation(std::size_t id, glm::vec3 *rotation);
            static void Transform_GetScale(std::size_t id, glm::vec3 *scale);
            static void Transform_SetScale(std::size_t id, glm::vec3 *scale);

            static void Camera_GetForward(std::size_t id, glm::vec3 *forward);
            static void Camera_SetForward(std::size_t id, glm::vec3 *forward);

            static bool IsKeyDown(int key);
            static void registerFunctions();
    };
}

#endif // SCRIPTGLUE_HPP
