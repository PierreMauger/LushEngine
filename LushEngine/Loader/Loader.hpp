#ifndef LOADER_HPP
#define LOADER_HPP

#include "Includes.hpp"
#include "Node.hpp"

namespace Lush
{
    class Loader : public virtual Node
    {
        private:
            /* data */
        public:
            Loader(std::shared_ptr<MessageBus> messageBus);
            ~Loader();
    };
}

#endif // LOADER_HPP
