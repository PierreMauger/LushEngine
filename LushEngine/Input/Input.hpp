#ifndef INPUT_HPP
#define INPUT_HPP

#include "Includes.hpp"
#include "Node.hpp"

namespace Lush
{
    class Input : public virtual Node
    {
        private:
            /* data */
        public:
            Input(std::shared_ptr<MessageBus> messageBus);
            ~Input();
    };
}

#endif // INPUT_HPP
