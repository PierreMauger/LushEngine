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
    std::cout << "Core::run()" << std::endl;
    // for (int i = 0; i < 3; i++)
        // std::this_thread::sleep_for(std::chrono::seconds(1));
}
