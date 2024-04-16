#ifndef SCRIPT_INSTANCE_HPP
#define SCRIPT_INSTANCE_HPP

#include "ECS/Entity/Entity.hpp"
#include "Includes.hpp"
#include "Script/ScriptClass.hpp"

namespace Lush
{
    class ScriptInstance
    {
        private:
            ScriptClass _class;
            MonoObject *_instance{};
            MonoMethod *_ctor{};
            MonoMethod *_onInit{};
            MonoMethod *_onUpdate{};
            MonoMethod *_onDestroy{};
            MonoMethod *_onCollisionEnter{};
            MonoMethod *_onCollisionStay{};
            MonoMethod *_onCollisionExit{};
            std::size_t _id{};
            std::unordered_map<std::string, std::any> _defaultFields;

        public:
            ScriptInstance() = default;
            ScriptInstance(ScriptClass &script, std::size_t id, std::unordered_map<std::string, std::any> &defaultFields);
            ~ScriptInstance() = default;

            MonoObject *getInstance();
            std::size_t getId() const;
            bool getFieldValueInternal(const std::string &name, void *value);
            void setFieldValueInternal(const std::string &name, void *value);

            template <typename T> T getFieldValue(const std::string &name)
            {
                char value[16];

                if (!this->getFieldValueInternal(name, &value))
                    return T();
                return *(T *)value;
            }

            template <typename T> void setFieldValue(const std::string &name, T value)
            {
                this->setFieldValueInternal(name, &value);
            }

            void onInit();
            void onUpdate(float time);
            void onDestroy();
            void onCollisionEnter(std::size_t id);
            void onCollisionStay(std::size_t id);
            void onCollisionExit(std::size_t id);
    };
}

#endif // SCRIPT_INSTANCE_HPP
