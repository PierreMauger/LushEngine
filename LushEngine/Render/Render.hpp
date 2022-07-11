#ifndef RENDER_HPP
#define RENDER_HPP

#include "Includes.hpp"
#include "Node.hpp"

namespace Lush
{
    class Render : public virtual Node
    {
        private:
            /* data */
        public:
            Render(std::shared_ptr<MessageBus> messageBus);
            ~Render();
    };
}

#endif // RENDER_HPP
