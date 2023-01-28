#include "Systems/Script/ScriptSystem.hpp"

using namespace Lush;

ScriptSystem::ScriptSystem(std::shared_ptr<Graphic> graphic, EntityManager &entityManager) : ASystem(60.0f), _graphic(graphic)
{
    try {
        this->initScriptDomain();
        this->loadBaseScript();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    this->_graphic->getScripts().push_back(ScriptClass("Spin", this->_domain));
    this->_graphic->getScripts().push_back(ScriptClass("Maxwell", this->_domain));
    this->_graphic->getScripts().push_back(ScriptClass("Controlable", this->_domain));
    ScriptGlue::registerFunctions();

    for (std::size_t i = 0; i < this->_graphic->getScripts().size(); i++)
        entityManager.addMaskCategory(ComponentType::COMPONENT_TYPE_COUNT << i);
}

ScriptSystem::~ScriptSystem()
{
    mono_jit_cleanup(this->_domain);
}

void ScriptSystem::update([[maybe_unused]] EntityManager &entityManager, [[maybe_unused]] ComponentManager &componentManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    if (this->_graphic->getPaused() || !this->_graphic->getRunning())
        return;
    for (auto &instance : this->_graphic->getInstances())
        instance.update(this->getDeltaTime());
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
