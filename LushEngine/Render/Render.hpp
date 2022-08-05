#ifndef RENDER_HPP
#define RENDER_HPP

#include <GL/glew.h>

#include "Camera.hpp"
#include "GLFW/glfw3.h"
#include "Includes.hpp"
#include "Node.hpp"
#include "RenderObject.hpp"
#include "StaticModel.hpp"
#include "AnimatedModel.hpp"

namespace Lush
{
    class Render : public virtual Node
    {
        private:
            GLFWwindow *_window;
            std::unique_ptr<Camera> _camera;
            std::map<int, std::unique_ptr<RenderObject>> _objects;

        public:
            Render(std::shared_ptr<MessageBus> messageBus);
            ~Render();
            void run();
            void draw();
    };
}

#endif // RENDER_HPP
