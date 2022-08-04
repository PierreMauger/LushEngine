#include "Node.hpp"

using namespace Lush;

Node::Node(std::shared_ptr<MessageBus> messageBus)
{
    this->_messageBus = messageBus;
    this->_messageBus->addReceiver();
    this->_running = true;

    this->_functionList.push_back(std::bind(&Node::receiveQuit, this, std::placeholders::_1));
    this->_functionList.push_back(std::bind(&Node::receiveSleepFor, this, std::placeholders::_1));
}

void Node::sendMessage(Message message)
{
    this->_messageBus->sendMessage(message);
}

void Node::receiveQuit([[maybe_unused]] Packet packet)
{
    this->_running = false;
}

void Node::receiveSleepFor(Packet packet)
{
    int value = 0;

    packet >> value;
    std::this_thread::sleep_for(std::chrono::seconds(value));
}
