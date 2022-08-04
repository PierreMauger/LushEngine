#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "Includes.hpp"
#include "Node.hpp"

namespace Lush
{
    class Audio : public virtual Node
    {
        private:
            /* data */
        public:
            Audio(std::shared_ptr<MessageBus> messageBus);
            ~Audio();
            void run();
    };
}

#endif // AUDIO_HPP
