#include "Systems/Script/ScriptSystem.hpp"

using namespace Lush;

ScriptSystem::ScriptSystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
{
    try {
        this->initScriptDomain();
        this->loadBaseScript();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    // TODO : Move this to ResourceManager
    std::map<std::string, ScriptClass> &scripts = this->_resourceManager->getScripts();
    std::map<std::string, File> files = this->_resourceManager->getFiles();

    scripts["Spin"] = ScriptClass(files["Spin"]);
    scripts["Maxwell"] = ScriptClass(files["MaxwellScript"]);
    scripts["Controlable"] = ScriptClass(files["Controlable"]);
    ScriptGlue::registerFunctions();
}

ScriptSystem::~ScriptSystem()
{
    mono_jit_cleanup(this->_domain);
}

void ScriptSystem::update([[maybe_unused]] EntityManager &entityManager, [[maybe_unused]] ComponentManager &componentManager, float deltaTime)
{
    if (this->_graphic->getPaused() || !this->_graphic->getRunning())
        return;
    if (!this->shouldUpdate(deltaTime))
        return;
    for (auto &instance : this->_resourceManager->getInstances())
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
    if (system("mcs -target:library -out:Resources/Scripts/Core.dll Resources/Scripts/Components.cs Resources/Scripts/InternalCalls.cs Resources/Scripts/Entity.cs"))
        throw std::runtime_error("mcs failed");
}
