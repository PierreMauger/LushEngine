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
            std::condition_variable _cv;
            std::mutex _mutex;
            int _count;

        public:
            Engine();
            ~Engine() = default;

            void run();

            template <typename T> void launchModule()
            {
                try {
                    T node(this->_messageBus);

                    if (this->_count == 4) {
                        this->_cv.notify_all();
                    } else {
                        std::unique_lock<std::mutex> lock(this->_mutex);
                        this->_count++;
                        this->_cv.wait(lock);
                    }
                    node.run();
                } catch (const std::exception &e) {
                    this->_messageBus->sendMessage(Message(Packet(), BaseCommand::QUIT, Module::BROADCAST));
                    std::cerr << "Thread aborted: " << e.what() << std::endl;
                }
            }
    };
}

#endif // ENGINE_HPP
