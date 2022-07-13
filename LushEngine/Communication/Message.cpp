#include "Message.hpp"

using namespace Lush;

Message::Message(Packet data, int status, Module target)
{
    this->_data = data;
    this->_status = status;
    this->_target = static_cast<int>(target);
}

Packet &Message::getData()
{
    return this->_data;
}

int Message::getStatus() const
{
    return this->_status;
}

int Message::getTarget() const
{
    return this->_target;
}
