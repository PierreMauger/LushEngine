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
    std::cout << "Input::run()" << std::endl;
    // for (int i = 0; i < 3; i++)
        // std::this_thread::sleep_for(std::chrono::seconds(1));
}
