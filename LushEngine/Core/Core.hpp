#ifndef CORE_HPP
#define CORE_HPP

#include "GameObject.hpp"
#include "Includes.hpp"
#include "Node.hpp"

namespace Lush
{
    class Core : public virtual Node
    {
        private:
            std::map<std::size_t, GameObject> _gameObjects;

        public:
            Core(std::shared_ptr<MessageBus> messageBus);
            ~Core();
            void run();

            void receiveObjectMove(Packet);
    };
}

#endif // CORE_HPP
