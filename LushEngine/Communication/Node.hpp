#ifndef NODE_HPP
#define NODE_HPP

#include "Includes.hpp"
#include "Message.hpp"
#include "MessageBus.hpp"
#include "Packet.hpp"

namespace Lush
{
    class Node
    {
        protected:
            std::shared_ptr<MessageBus> _messageBus;
            std::vector<std::function<void(Packet)>> _functionList;
            bool _running;

            std::function<void(Message)> getNotify();
            void sendMessage(Message);
            void onNotify(Message);
            virtual void run() = 0;

            void receiveQuit(Packet);

        public:
            Node(std::shared_ptr<MessageBus> messageBus);
            virtual ~Node() = default;
    };
}

#endif // NODE_HPP
