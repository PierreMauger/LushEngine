#ifndef CONTROLSYSTEM_HPP
#define CONTROLSYSTEM_HPP

#include "GLFW/glfw3.h"
#include "Includes.hpp"
#include "System/ISystem.hpp"

namespace Lush
{
    class ControlSystem : public ISystem
    {
        private:
            std::shared_ptr<GLFWwindow> _window;

        public:
            ControlSystem(std::shared_ptr<GLFWwindow> window);
            ~ControlSystem() = default;

            void update(ComponentManager &componentManager, EntityManager &entityManager);
    };
}

#endif // CONTROLSYSTEM_HPP
