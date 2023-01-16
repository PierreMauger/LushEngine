#include "Systems/Script/ScriptSystem.hpp"

using namespace Lush;

ScriptSystem::ScriptSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager)
{
    this->_graphic = graphic;

    try {
        this->initScriptDomain();
        this->loadBaseScript();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    // this->_scripts.push_back(std::make_unique<ScriptClass>("Spin", this->_domain));
    // this->_graphic->getScriptNames().push_back("Spin");
    // this->_scripts.push_back(std::make_unique<ScriptClass>("Boing", this->_domain));
    // this->_graphic->getScriptNames().push_back("Boing");
    ScriptGlue::registerFunctions();

    for (std::size_t i = 0; i < this->_scripts.size(); i++)
        entityManager.addMaskCategory(ComponentType::COMPONENT_TYPE_COUNT << i);
}

ScriptSystem::~ScriptSystem()
{
    this->_instances.clear();
    this->_scripts.clear();
    mono_jit_cleanup(this->_domain);
}

void ScriptSystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    if (this->buttonChanged()) {
        if (!this->_graphic->getRunning()) {
            this->_instances.clear();
            return;
        }
        for (std::size_t i = 0; i < this->_scripts.size(); i++)
            for (auto id : entityManager.getMaskCategory(ComponentType::COMPONENT_TYPE_COUNT << i))
                this->_instances.push_back(std::make_unique<ScriptInstance>(*this->_scripts[i], id));
        for (auto &instance : this->_instances)
            instance->init();
    }
    if (this->_graphic->getPaused() || !this->_graphic->getRunning())
        return;
    float deltaTime = glfwGetTime() - this->_graphic->getLastTime();
    for (auto &instance : this->_instances)
        instance->update(deltaTime);
}

bool ScriptSystem::buttonChanged()
{
    if (this->_initialized && !this->_graphic->getRunning()) {
        this->_initialized = false;
        return true;
    } else if (!this->_initialized && this->_graphic->getRunning()) {
        this->_initialized = true;
        return true;
    }
    return false;
}

void ScriptSystem::initScriptDomain()
{
    unsetenv("TERM");
    this->_domain = mono_jit_init("LushJIT");
    if (!this->_domain)
        throw std::runtime_error("mono_jit_init failed");
}

void ScriptSystem::loadBaseScript()
{
    if (system("mcs -target:library -out:Resources/Scripts/Base.dll Resources/Scripts/Components.cs Resources/Scripts/InternalCalls.cs Resources/Scripts/Entity.cs"))
        throw std::runtime_error("mcs failed");
}
