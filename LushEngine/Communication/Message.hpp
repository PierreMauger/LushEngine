#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "Includes.hpp"
#include "Packet.hpp"

namespace Lush
{
    class Message
    {
        private:
            Packet _data;
            int _status;
            int _target;

        public:
            Message(Packet data, int status, int target = -1);
            ~Message() = default;

            Packet &getData();
            int getStatus() const;
            int getTarget() const;
    };
}

#endif // MESSAGE_HPP
