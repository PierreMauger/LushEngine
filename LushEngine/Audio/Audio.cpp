#include "Audio.hpp"

using namespace Lush;

Audio::Audio(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
}

Audio::~Audio()
{
}

void Audio::run()
{
    while (this->_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        this->_messageBus->notify(Module::AUDIO);
    }
}
