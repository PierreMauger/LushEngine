#ifndef SCRIPTGLUE_HPP
#define SCRIPTGLUE_HPP

#include "Engine.hpp"
#include "Includes.hpp"
#include "mono/jit/jit.h"

namespace Lush
{
    class ScriptGlue
    {
        public:
            static void Console_Log(std::size_t id, MonoString *message);
            static void Transform_Get(std::size_t id, glm::vec3 *outTranslation);
            static void Transform_Set(std::size_t id, glm::vec3 *translation);
            static void registerFunctions();
    };
}

#endif // SCRIPTGLUE_HPP
