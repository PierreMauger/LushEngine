#include "MessageBus.hpp"

using namespace Lush;

void MessageBus::addReceiver()
{
    std::unique_lock<std::mutex> lock(this->_copyLock);
    this->_queues.push_back(SafeQueue<Message>());
    lock.unlock();
}

void MessageBus::notify(Module module, std::vector<std::function<void(Packet)>> functionList)
{
    if (static_cast<std::size_t>(module) >= this->_queues.size())
        return;
    SafeQueue<Message> &queue = this->_queues[static_cast<int>(module)];

    while (queue.size()) {
        Message message = queue.front();
        if (static_cast<int>(functionList.size()) > message.getStatus())
            functionList[message.getStatus()](message.getData());
        queue.pop();
    }
}

void MessageBus::sendMessage(Message message)
{
    if (message.getTarget() == -1) {
        for (auto &queue : this->_queues)
            queue.push(message);
    } else if (message.getTarget() >= 0 && message.getTarget() < static_cast<int>(this->_queues.size()))
        this->_queues[message.getTarget()].push(message);
    else
        std::cerr << "MessageBus::sendMessage: invalid target" << std::endl;
}
