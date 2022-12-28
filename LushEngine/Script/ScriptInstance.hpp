#ifndef SCRIPTINSTANCE_HPP
#define SCRIPTINSTANCE_HPP

#include "Includes.hpp"
#include "Script/Script.hpp"

namespace Lush
{
    class ScriptInstance
    {
        public:
            ScriptInstance(Script &script, std::size_t id);
            ~ScriptInstance() = default;

            void init();
            void update(float time);

        private:
            MonoObject *_instance;
            MonoMethod *_ctor;
            MonoMethod *_onInit;
            MonoMethod *_onUpdate;
    };
}

#endif // SCRIPTINSTANCE_HPP
