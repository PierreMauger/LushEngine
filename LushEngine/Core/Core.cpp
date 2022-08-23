#include "Core.hpp"

using namespace Lush;

Core::Core(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
    this->_functionList.push_back(std::bind(&Core::receiveScenes, this, std::placeholders::_1));
    this->_functionList.push_back(std::bind(&Core::receiveMoveObject, this, std::placeholders::_1));

    this->_actScene = 0;
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

void Core::loadScene()
{
    Packet packet;

    for (auto &[key, object] : this->_scenes["scene1"].getObjects())
        packet << key << object;
    this->sendMessage(Message(packet, RenderCommand::ADD_OBJECTS, Module::RENDER));
}

void Core::receiveMoveObject(Packet packet)
{
    int id = 0;
    glm::vec3 newPosition(0.0f);
    auto &objects = this->_scenes["scene1"].getObjects();

    packet >> id >> newPosition;
    if (objects.find(id) != objects.end())
        objects[id].setPosition(newPosition);
}

void Core::receiveScenes(Packet packet)
{
    while (!packet.empty()) {
        std::string sceneName;
        std::size_t size;
        std::map<std::size_t, GameObject> objects;

        packet >> sceneName >> size;
        for (std::size_t i = 0; i < size; i++)
            packet >> objects[i];
        this->_scenes[sceneName] = Scene(objects);
    }
    this->loadScene();
}
