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
#include "ImGui/imgui_markdown.h"
#include "ImGui/imgui_toast.h"
#include "Includes.hpp"
#include "ResourceManager.hpp"

#define BUTTON_COLOR ImVec4(0.26f, 0.59f, 0.98f, 0.40f)
#define BUTTON_COLOR_SELECTED ImVec4(0.26f, 0.59f, 0.98f, 0.80f)
#define BUTTON_COLOR_DISABLED ImVec4(0.26f, 0.59f, 0.98f, 0.10f)
#define BUTTON_COLOR_RED ImVec4(0.98f, 0.26f, 0.26f, 0.40f)

namespace Lush
{
    struct ProjectSettings {
            std::string rootPath;
            std::string buildPath;
            std::string execName;
            glm::vec3 color = glm::vec3(0.26f, 0.59f, 0.98f);
            std::string iconName = "None";
            std::vector<std::string> hiddenScenes;

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar & rootPath;
                ar & buildPath;
                ar & execName;
                ar & color;
                ar & iconName;
                ar & hiddenScenes;
            }
    };

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
            bool _showProjectManager = false;
            bool _showBuildBrowser = false;
            bool _showRootBrowser = false;

            bool _resetLayout = false;
            bool _singleFrame = false;
            ImGuizmo::OPERATION _currentOperation = ImGuizmo::OPERATION::TRANSLATE;
            ImGuizmo::MODE _currentMode = ImGuizmo::MODE::LOCAL;

            std::vector<std::pair<int, std::string>> _consoleBuffer;

            std::string _fileBrowserPath;
            std::string _fileExplorerPath;
            std::unordered_map<std::string, ProjectSettings> _projectSettings;
            std::string _currentProject;
            std::string _editingProject;

            void loadProjectSettings();
            void saveProjectSettings();

            void handleConsole();

            static void setDock();
            void drawMenuBar();
            void drawActionBar(std::shared_ptr<EntityManager> &entityManager);

            void drawSceneHierarchy(std::shared_ptr<EntityManager> &entityManager);
            void drawProperties(std::shared_ptr<EntityManager> &entityManager);
            void drawTools();
            void drawConsole();
            void drawGame();
            void drawScene(std::shared_ptr<EntityManager> &entityManager);
            bool drawGuizmo(std::shared_ptr<EntityManager> &entityManager);
            void drawFileExplorer();
            void drawProfiler();
            void drawProjectManager();
            std::size_t getPhysicInstanceIndex(std::size_t entityId);
            bool drawTextureSelect(const std::string &fieldName, std::string &texture);

            bool openFolderBrowser(std::string title, std::string &path, bool &modal);

            void build();

        public:
            GUISystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager);
            ~GUISystem() override;

            void update(std::shared_ptr<EntityManager> &entityManager, float deltaTime) override;
    };
}

#endif // GUI_SYSTEM_HPP
