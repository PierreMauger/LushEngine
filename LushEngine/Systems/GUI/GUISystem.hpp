#ifndef GUISYSTEM_HPP
#define GUISYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "GLFW/glfw3.h"
#include "Includes.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define FORMAT_NAME(name) name + std::to_string(std::stoi(name)).length()

namespace Lush
{
    class GUISystem : public ISystem
    {
        private:
            std::shared_ptr<GLFWwindow> _window;
            bool _showEntityManager = true;
            bool _showEntityDetails = false;
            std::size_t _selectedEntity = 0;

            void drawEntityManager(ComponentManager &componentManager, EntityManager &entityManager);
            void drawEntityDetails(ComponentManager &componentManager, EntityManager &entityManager);
            std::string formatBool(std::size_t value, std::size_t size);

        public:
            GUISystem(std::shared_ptr<GLFWwindow> window);
            ~GUISystem();

            void update(ComponentManager &componentManager, EntityManager &entityManager);
    };
}

#endif // GUISYSTEM_HPP
