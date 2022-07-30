#ifndef MESSAGE_BUS_HPP
#define MESSAGE_BUS_HPP

#include "Includes.hpp"
#include "Message.hpp"
#include "SafeQueue.cpp"

namespace Lush
{
    class MessageBus
    {
        private:
            std::vector<std::function<void(Message)>> _functionList;
            std::vector<SafeQueue<Message>> _queues;
            std::mutex _copyLock;

        public:
            void addReceiver(std::function<void(Message)> function);
            void notify(Module module);
            void sendMessage(Message message);
    };
}

#endif // MESSAGE_BUS_HPP