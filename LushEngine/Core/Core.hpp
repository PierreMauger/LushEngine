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
            std::map<std::string, Scene> _scenes;
            std::string _actScene;

        public:
            Core(std::shared_ptr<MessageBus> messageBus);
            ~Core();
            void run();
            void loadScene();

            void receiveMoveObject(Packet);
            void receiveScenes(Packet);
            void receiveLoadScene(Packet);
    };
}

#endif // CORE_HPP
