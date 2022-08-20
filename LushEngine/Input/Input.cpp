#include "Input.hpp"

using namespace Lush;

Input::Input(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
}

Input::~Input()
{
}

void Input::run()
{
    while (this->_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        this->_messageBus->notify(Module::INPUT, this->_functionList);
        if (!this->_launched)
            continue;
    }
}
