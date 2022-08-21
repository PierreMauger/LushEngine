#ifndef CORE_HPP
#define CORE_HPP

#include "GameObject.hpp"
#include "Includes.hpp"
#include "Node.hpp"
#include "Scene.hpp"

namespace Lush
{
    class Core : public virtual Node
    {
        private:
            std::map<std::size_t, std::unique_ptr<GameObject>> _objects;
            std::map<std::size_t, Scene> _scenes;

        public:
            Core(std::shared_ptr<MessageBus> messageBus);
            ~Core();
            void run();
            void loadScene();

            void receiveMoveObject(Packet);
            void receiveScenes(Packet);
    };
}

#endif // CORE_HPP
