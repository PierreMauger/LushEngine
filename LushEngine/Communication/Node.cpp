#include "Node.hpp"

using namespace Lush;

Node::Node(std::shared_ptr<MessageBus> messageBus)
{
    this->_messageBus = messageBus;
    this->_messageBus->addReceiver(this->getNotify());
    this->_running = true;

    this->_functionList.push_back(std::bind(&Node::receiveQuit, this, std::placeholders::_1));
}

std::function<void(Message)> Node::getNotify()
{
    auto messageListener = [=](Message message) -> void { this->onNotify(message); };

    return messageListener;
}

void Node::sendMessage(Message message)
{
    this->_messageBus->sendMessage(message);
}

void Node::onNotify(Message message)
{
    Packet data = message.getData();
    int status = message.getStatus();

    if (status >= 0 && status < static_cast<int>(this->_functionList.size()))
        this->_functionList[status](data);
}

void Node::receiveQuit([[maybe_unused]] Packet packet)
{
    this->_running = false;
}
