#ifndef GUISYSTEM_HPP
#define GUISYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "Includes.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"

#define FORMAT_NAME(name) name + std::to_string(std::stoi(name)).length()

namespace Lush
{
    class GUISystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            bool _showEntityManager = true;
            bool _showEntityDetails = false;
            std::size_t _selectedEntity = 0;

            void drawMenuBar();
            void drawActionBar();

            void drawEntityManager(EntityManager &entityManager, ComponentManager &componentManager);
            void drawEntityDetails(EntityManager &entityManager, ComponentManager &componentManager);
            void drawGuizmo(EntityManager &entityManager, ComponentManager &componentManager);
            std::string formatBool(std::size_t value, std::size_t size);

        public:
            GUISystem(std::shared_ptr<Graphic> graphic);
            ~GUISystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // GUISYSTEM_HPP
