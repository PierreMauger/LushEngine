#include "Loader.hpp"

using namespace Lush;

Loader::Loader(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
}

Loader::~Loader()
{
}

void Loader::run()
{
    while (this->_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        this->_messageBus->notify(Module::LOADER, this->_functionList);
    }
}
