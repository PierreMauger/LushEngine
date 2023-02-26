#ifndef GUISYSTEM_HPP
#define GUISYSTEM_HPP

#include "ComponentTypes.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "ResourceManager.hpp"
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
    class GUISystem : public ASystem
    {
        private:
            std::shared_ptr<Graphic> _graphic;
            std::shared_ptr<ResourceManager> _resourceManager;

            bool _showSceneHierarchy = true;
            bool _showProperties = true;
            bool _showTools = true;
            bool _showConsole = true;
            bool _showGame = true;
            bool _showScene = true;
            bool _showFileExplorer = true;
            bool _showProfiler = true;
            bool _reloading = false;
            bool _singleFrame = false;
            ImGuizmo::OPERATION _currentOperation = ImGuizmo::OPERATION::TRANSLATE;
            ImGuizmo::MODE _currentMode = ImGuizmo::MODE::LOCAL;

            EntityManager _entityManagerCopy;
            ComponentManager _componentManagerCopy;

            std::string _consoleBuffer;

            std::string _fileExplorerPath;
            std::string _fileExplorerRootPath;

            void setDock();
            void drawMenuBar();
            void drawActionBar(EntityManager &entityManager, ComponentManager &componentManager);

            void drawSceneHierarchy(EntityManager &entityManager, ComponentManager &componentManager);
            void drawProperties(EntityManager &entityManager, ComponentManager &componentManager);
            void drawTools();
            void drawConsole();
            void drawGame();
            void drawScene(EntityManager &entityManager, ComponentManager &componentManager);
            bool drawGuizmo(EntityManager &entityManager, ComponentManager &componentManager);
            void drawFiles();
            void drawProfiler();
            std::string formatBinary(std::size_t value, std::size_t size);
            std::size_t getScriptInstanceIndex(std::size_t entityId);

        public:
            GUISystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~GUISystem();

            void update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime);
    };
}

#endif // GUISYSTEM_HPP
