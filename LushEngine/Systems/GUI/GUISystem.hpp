#ifndef GUISYSTEM_HPP
#define GUISYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Graphic/Graphic.hpp"
#include "ImGui/IconsFontAwesome5.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/ImGuizmo.h"
#include "Includes.hpp"

#define FORMAT_NAME(name) name + std::to_string(std::stoi(name)).length()
#define BUTTON_COLOR ImVec4(0.26f, 0.59f, 0.98f, 0.40f)
#define BUTTON_COLOR_SELECTED ImVec4(0.26f, 0.59f, 0.98f, 0.80f)

namespace Lush
{
    class GUISystem : public ISystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            bool _showSceneHierarchy = true;
            bool _showProperties = true;
            bool _showTools = true;
            bool _showConsole = true;
            bool _showScene = true;
            bool _showGame = true;
            bool _showFileExplorer = true;
            bool _showProfiler = true;
            bool _reloading = false;
            std::size_t _selectedEntity = 0;
            ImGuizmo::OPERATION _currentOperation = ImGuizmo::OPERATION::TRANSLATE;
            ImGuizmo::MODE _currentMode = ImGuizmo::MODE::LOCAL;

            EntityManager _entityManagerCopy;
            ComponentManager _componentManagerCopy;

            std::string _consoleBuffer;

            void setDock();
            void drawMenuBar();
            void drawActionBar(EntityManager &entityManager, ComponentManager &componentManager);

            void drawSceneHierarchy(EntityManager &entityManager, ComponentManager &componentManager);
            void drawProperties(EntityManager &entityManager, ComponentManager &componentManager);
            void drawTools();
            void drawConsole();
            void drawScene(EntityManager &entityManager, ComponentManager &componentManager);
            void drawGuizmo(EntityManager &entityManager, ComponentManager &componentManager);
            void drawGame();
            void drawFiles();
            void drawProfiler();
            std::string formatBinary(std::size_t value, std::size_t size);

        public:
            GUISystem(std::shared_ptr<Graphic> graphic);
            ~GUISystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // GUISYSTEM_HPP
