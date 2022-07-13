#include "Render.hpp"

using namespace Lush;

Render::Render(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
}

Render::~Render()
{
}

void Render::run()
{
    std::cout << "Render::run()" << std::endl;
    // for (int i = 0; i < 3; i++)
        // std::this_thread::sleep_for(std::chrono::seconds(1));
}
