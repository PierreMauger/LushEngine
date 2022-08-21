#include "Core.hpp"

using namespace Lush;

Core::Core(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
    this->_functionList.push_back(std::bind(&Core::receiveMoveObject, this, std::placeholders::_1));
    this->_functionList.push_back(std::bind(&Core::receiveScenes, this, std::placeholders::_1));

    this->_objects[0] = std::unique_ptr<GameObject>(new GameObject(0, "Cube", glm::vec3(0.0f)));
    this->_objects[1] = std::unique_ptr<GameObject>(new GameObject(0, "Cube", glm::vec3(2.0f, 0.0f, 0.5f)));
}

Core::~Core()
{
}

void Core::run()
{
    this->loadScene();

    while (this->_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        this->_messageBus->notify(Module::CORE, this->_functionList);
        if (!this->_launched)
            continue;
    }
}

void Core::loadScene()
{
    Packet packet;

    for (auto &[key, object] : this->_objects)
        packet << key << *object;
    this->sendMessage(Message(packet, RenderCommand::ADD_OBJECTS, Module::RENDER));
}

void Core::receiveMoveObject(Packet packet)
{
    int id = 0;
    glm::vec3 newPosition(0.0f);

    packet >> id >> newPosition;
    if (this->_objects.find(id) != this->_objects.end())
        this->_objects[id]->setPosition(newPosition);
}

void Core::receiveScenes(Packet packet)
{
    int temp;

    packet >> temp;
    // TODO receive scenes from packet
}
