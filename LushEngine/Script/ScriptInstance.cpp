#include "Script/ScriptInstance.hpp"

using namespace Lush;

ScriptInstance::ScriptInstance(Script &script, std::size_t id)
{
    this->_instance = mono_object_new(script.getDomain(), script.getClass());
    this->_ctor = script.getMethod("ctor");
    this->_onInit = script.getMethod("onInit");
    this->_onUpdate = script.getMethod("onUpdate");

    if (this->_ctor) {
        void *args[1];
        args[0] = &id;
        mono_runtime_invoke(this->_ctor, this->_instance, args, nullptr);
    }
}

void ScriptInstance::init()
{
    if (this->_onInit) {
        void *args[0];
        mono_runtime_invoke(this->_onInit, this->_instance, args, nullptr);
    }
}

void ScriptInstance::update(float time)
{
    if (this->_onUpdate) {
        void *args[1];
        args[0] = &time;
        mono_runtime_invoke(this->_onUpdate, this->_instance, args, nullptr);
    }
}
