#ifndef SCRIPTINSTANCE_HPP
#define SCRIPTINSTANCE_HPP

#include "Includes.hpp"
#include "Script/ScriptClass.hpp"

namespace Lush
{
    class ScriptInstance
    {
        public:
            ScriptInstance(ScriptClass &script, std::size_t id);
            ~ScriptInstance() = default;

            ScriptClass &getClass();

            void init();
            void update(float time);

        private:
            ScriptClass _class;
            MonoObject *_instance;
            MonoMethod *_ctor;
            MonoMethod *_onInit;
            MonoMethod *_onUpdate;
    };
}

#endif // SCRIPTINSTANCE_HPP
