#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ComponentTypes.hpp"
#include "ECS/ECS.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"

namespace Lush
{
    class Engine
    {
        private:
            ECS _ecs;
            std::shared_ptr<Graphic> _graphic;

        public:
            Engine();
            ~Engine();

            void loadScene();
            void run();
            void updateMouse();
    };
}

#endif // ENGINE_HPP
