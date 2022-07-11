#include "Core.hpp"

using namespace Lush;

Core::Core(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
}

Core::~Core()
{
}
