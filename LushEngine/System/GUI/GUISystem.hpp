#ifndef GUISYSTEM_HPP
#define GUISYSTEM_HPP

#include "GLFW/glfw3.h"
#include "Includes.hpp"
#include "System/ISystem.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define FORMAT_NAME(name) name + std::strlen(name) / 10 + 1

namespace Lush
{
    class GUISystem : public ISystem
    {
        private:
            std::shared_ptr<GLFWwindow> _window;
            bool _showEntityManager = true;
            bool _showEntityDetails = false;
            std::size_t _selectedEntity = 0;

        public:
            GUISystem(std::shared_ptr<GLFWwindow> window);
            ~GUISystem();

            void update(ComponentManager &componentManager, EntityManager &entityManager);

            void drawEntityManager(ComponentManager &componentManager, EntityManager &entityManager);
            void drawEntityDetails(ComponentManager &componentManager, EntityManager &entityManager);

            std::string formatBool(std::size_t value, std::size_t size);
    };
}

#endif // GUISYSTEM_HPP
