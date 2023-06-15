#ifndef GUI_SYSTEM_HPP
#define GUI_SYSTEM_HPP

#include "ECS/Component/Component.hpp"
#include "ECS/System/ASystem.hpp"
#include "Graphic.hpp"
#include "ImGui/IconsFontAwesome5.h"
#include "ImGui/ImGuizmo.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_toast.h"
#include "Includes.hpp"
#include "ResourceManager.hpp"

#define BUTTON_COLOR ImVec4(0.26f, 0.59f, 0.98f, 0.40f)
#define BUTTON_COLOR_SELECTED ImVec4(0.26f, 0.59f, 0.98f, 0.80f)
#define BUTTON_COLOR_DISABLED ImVec4(0.26f, 0.59f, 0.98f, 0.10f)

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
            bool _showProjectBrowser = false;
            bool _showBuildBrowser = false;
            bool _resetLayout = false;
            bool _singleFrame = false;
            ImGuizmo::OPERATION _currentOperation = ImGuizmo::OPERATION::TRANSLATE;
            ImGuizmo::MODE _currentMode = ImGuizmo::MODE::LOCAL;

            EntityManager _entityManagerCopy;

            std::string _consoleBuffer;

            std::string _fileBrowserPath;

            std::string _currentPath;
            std::string _projectPath;
            std::string _buildPath;

            void handleConsole();

            static void setDock();
            void drawMenuBar();
            void drawActionBar(EntityManager &entityManager);

            void drawSceneHierarchy(EntityManager &entityManager);
            void drawProperties(EntityManager &entityManager);
            void drawTools();
            void drawConsole();
            void drawGame();
            void drawScene(EntityManager &entityManager);
            bool drawGuizmo(EntityManager &entityManager);
            void drawFileExplorer();
            void drawProfiler();
            void drawProjectBrowser();
            void drawBuildBrowser();
            std::size_t getScriptInstanceIndex(std::size_t entityId);
            void drawTextureSelect(const std::string &fieldName, std::string &texture);

            void build();

        public:
            GUISystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~GUISystem() override;

            void update(EntityManager &entityManager, float deltaTime) override;
    };
}

#endif // GUI_SYSTEM_HPP
