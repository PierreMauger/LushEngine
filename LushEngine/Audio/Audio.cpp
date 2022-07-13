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
    std::cout << "Audio::run()" << std::endl;
    // for (int i = 0; i < 3; i++)
        // std::this_thread::sleep_for(std::chrono::seconds(1));
}
