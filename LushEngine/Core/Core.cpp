#include "Core.hpp"

using namespace Lush;

Core::Core(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
}

Core::~Core()
{
}

void Core::run()
{
    while (this->_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        this->_messageBus->notify(Module::CORE, this->_functionList);
    }
}
