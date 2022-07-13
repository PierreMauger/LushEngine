#include "MessageBus.hpp"

using namespace Lush;

void MessageBus::addReceiver(std::function<void(Message)> function)
{
    this->_functionList.push_back(function);
    this->_queues.push_back(SafeQueue<Message>());
}

void MessageBus::notify(int module)
{
    SafeQueue<Message> &queue = this->_queues[module];
    auto &function = this->_functionList[module];

    while (!queue.size() != 0) {
        function(queue.front());
        queue.pop();
    }
}

void MessageBus::sendMessage(Message message)
{
    if (message.getTarget() == -1)
        for (auto &queue : this->_queues)
            queue.push(message);
    else
        this->_queues[message.getTarget()].push(message);
}

void MessageBus::safeCopy()
{
    std::unique_lock<std::mutex> lock(this->_copyLock);
}
