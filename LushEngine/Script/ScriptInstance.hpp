#ifndef SCRIPTINSTANCE_HPP
#define SCRIPTINSTANCE_HPP

#include "Includes.hpp"
#include "Script/ScriptClass.hpp"

namespace Lush
{
    class ScriptInstance
    {
        private:
            ScriptClass _class;
            MonoObject *_instance;
            MonoMethod *_ctor;
            MonoMethod *_onInit;
            MonoMethod *_onUpdate;
            std::size_t _id;

        public:
            ScriptInstance(ScriptClass &script, std::size_t id);
            ~ScriptInstance() = default;

            ScriptClass &getClass();
            std::size_t getId();
            bool getFieldValueInternal(std::string name, void *value);
            void setFieldValueInternal(std::string name, void *value);

            template <typename T> T getFieldValue(std::string name)
            {
                char value[16];

                if (!this->getFieldValueInternal(name, &value))
                    return T();
                return *(T *)value;
            }
            template <typename T> void setFieldValue(std::string name, T value)
            {
                this->setFieldValueInternal(name, &value);
            }

            void init();
            void update(float time);
    };
}

#endif // SCRIPTINSTANCE_HPP
