#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "GLFW/glfw3.h"
#include "Includes.hpp"
#include "System/ISystem.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Lush
{
    class RenderSystem : public ISystem
    {
        private:
            std::shared_ptr<GLFWwindow> _window;

        public:
            RenderSystem(std::shared_ptr<GLFWwindow> window);
            ~RenderSystem();

            void update(ComponentManager &componentManager, EntityManager &entityManager);
    };
}

#endif // RENDERSYSTEM_HPP
