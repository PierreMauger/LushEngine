#ifndef CORE_HPP
#define CORE_HPP

#include "Includes.hpp"
#include "Node.hpp"

namespace Lush
{
    class Core : public virtual Node
    {
        private:
            /* data */
        public:
            Core(std::shared_ptr<MessageBus> messageBus);
            ~Core();
            void run();
    };
}

#endif // CORE_HPP
