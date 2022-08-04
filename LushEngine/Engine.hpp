#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Audio.hpp"
#include "Core.hpp"
#include "Includes.hpp"
#include "Input.hpp"
#include "Loader.hpp"
#include "MessageBus.hpp"
#include "Node.hpp"
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
                try {
                    T node(this->_messageBus);
                    node.run();
                } catch (const std::exception &e) {
                    this->_messageBus->sendMessage(Message(Packet(), BaseCommand::QUIT, Module::BROADCAST));
                    std::cerr << "Thread aborted: " << e.what() << std::endl;
                }
            }
    };
}

#endif // ENGINE_HPP
