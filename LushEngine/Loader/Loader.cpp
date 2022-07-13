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
    std::cout << "Loader::run()" << std::endl;
    // for (int i = 0; i < 3; i++)
        // std::this_thread::sleep_for(std::chrono::seconds(1));
}
