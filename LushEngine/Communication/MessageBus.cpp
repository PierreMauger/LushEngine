#include "MessageBus.hpp"

using namespace Lush;

void MessageBus::addReceiver(std::function<void(Message)> function)
{
    std::unique_lock<std::mutex> lock(this->_copyLock);
    this->_functionList.push_back(function);
    this->_queues.push_back(SafeQueue<Message>());
}

void MessageBus::notify(Module module)
{
    SafeQueue<Message> &queue = this->_queues[static_cast<int>(module)];
    auto &function = this->_functionList[static_cast<int>(module)];

    while (queue.size()) {
        function(queue.front());
        queue.pop();
    }
}

void MessageBus::sendMessage(Message message)
{
    if (message.getTarget() == -1)
        for (auto &queue : this->_queues)
            queue.push(message);
    else if (message.getTarget() >= 0 && message.getTarget() < static_cast<int>(this->_queues.size()))
        this->_queues[message.getTarget()].push(message);
}
