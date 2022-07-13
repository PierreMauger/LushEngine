#include "Engine.hpp"

using namespace Lush;

Engine::Engine() : _messageBus(std::make_shared<MessageBus>())
{
}

Engine::~Engine()
{
}

void Engine::run()
{
    std::thread audioThread(&Engine::launchModule<Audio>, this);
    std::thread coreThread(&Engine::launchModule<Core>, this);
    std::thread inputThread(&Engine::launchModule<Input>, this);
    std::thread loaderThread(&Engine::launchModule<Loader>, this);
    std::thread renderThread(&Engine::launchModule<Render>, this);

    audioThread.join();
    coreThread.join();
    inputThread.join();
    loaderThread.join();
    renderThread.join();
}
