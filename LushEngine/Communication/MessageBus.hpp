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

        public:
            std::mutex _copyLock;
            void addReceiver(std::function<void(Message)> function);
            void notify(int module);
            void sendMessage(Message message);
            void safeCopy();
    };
}

#endif // MESSAGE_BUS_HPP
