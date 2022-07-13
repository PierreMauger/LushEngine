#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Includes.hpp"
#include "MessageBus.hpp"
#include "Node.hpp"
#include "Audio.hpp"
#include "Core.hpp"
#include "Input.hpp"
#include "Loader.hpp"
#include "Render.hpp"

namespace Lush
{
    class Engine
    {
        private:
            std::shared_ptr<MessageBus> _messageBus;

        public:
            Engine();
            ~Engine();

            void run();
            template <typename T> void launchModule()
            {
                T node(this->_messageBus);
                node.run();
            }
    };
}

#endif // ENGINE_HPP
