#include "Core.hpp"

using namespace Lush;

Core::Core(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
    this->_functionList.push_back(std::bind(&Core::receiveObjectMove, this, std::placeholders::_1));
}

Core::~Core()
{
}

void Core::run()
{
    while (this->_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        this->_messageBus->notify(Module::CORE, this->_functionList);
        if (!this->_launched)
            continue;
    }
}

void Core::receiveObjectMove(Packet packet)
{
    int id = 0;
    glm::vec3 newPosition(0.0f);

    packet >> id >> newPosition;
    if (this->_gameObjects.find(id) != this->_gameObjects.end())
        this->_gameObjects[id].setPosition(newPosition);
}
