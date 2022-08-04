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
            std::vector<SafeQueue<Message>> _queues;
            std::mutex _copyLock;

        public:
            void addReceiver();
            void notify(Module module, std::vector<std::function<void(Packet)>> functionList);
            void sendMessage(Message message);
    };
}

#endif // MESSAGE_BUS_HPP
