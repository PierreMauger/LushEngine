#ifndef RENDER_HPP
#define RENDER_HPP

#include "Camera.hpp"
#include "GLFW/glfw3.h"
#include "Includes.hpp"
#include "Node.hpp"

namespace Lush
{
    class Render : public virtual Node
    {
        private:
            GLFWwindow *_window;
            std::unique_ptr<Camera> _camera;

        public:
            Render(std::shared_ptr<MessageBus> messageBus);
            ~Render();
            void run();
    };
}

#endif // RENDER_HPP
