#include "Systems/GUI/GUISystem.hpp"

using namespace Lush;

GUISystem::GUISystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager) : ASystem(60.0f), _graphic(graphic), _resourceManager(resourceManager)
{
    if (!IMGUI_CHECKVERSION())
        throw std::runtime_error("ImGui version is invalid");
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(this->_graphic->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 410");

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NoMouseCursorChange;
    io.Fonts->AddFontDefault();

    // merge in icons from Font Awesome
    static const ImWchar iconsRanges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig iconsConfig;
    iconsConfig.MergeMode = true;
    iconsConfig.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("Resources/Fonts/" FONT_ICON_FILE_NAME_FAS, 12.0f, &iconsConfig, iconsRanges);

    ImGuizmo::Enable(true);
    ImGuizmo::AllowAxisFlip(false);

    this->_fileExplorerPath = std::filesystem::current_path().string();
    this->_fileExplorerRootPath = std::filesystem::current_path().string();
}

GUISystem::~GUISystem()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUISystem::update(EntityManager &entityManager, ComponentManager &componentManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    if (this->_singleFrame) {
        this->_singleFrame = false;
        this->_graphic->setPaused(true);
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    this->setDock();
    this->drawMenuBar();
    this->drawActionBar(entityManager, componentManager);

    if (this->_showSceneHierarchy)
        this->drawSceneHierarchy(entityManager, componentManager);
    if (this->_showProperties)
        this->drawProperties(entityManager, componentManager);
    if (this->_showTools)
        this->drawTools();
    if (this->_showConsole)
        this->drawConsole();
    if (this->_showGame)
        this->drawGame();
    if (this->_showScene)
        this->drawScene(entityManager, componentManager);
    if (this->_showFileExplorer)
        this->drawFiles();
    if (this->_showProfiler)
        this->drawProfiler();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (this->_reloading) {
        ImGui::LoadIniSettingsFromDisk("Resources/Config/dockConfig.ini");
        this->_reloading = false;
        this->_showSceneHierarchy = true;
        this->_showProperties = true;
        this->_showTools = true;
        this->_showConsole = true;
        this->_showScene = true;
        this->_showGame = true;
        this->_showFileExplorer = true;
        this->_showProfiler = true;
    }
}

void GUISystem::setDock()
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos({viewport->Pos.x, viewport->Pos.y + 19}); // 19 is the height of the action bar
    ImGui::SetNextWindowSize({viewport->Size.x, viewport->Size.y - 19});
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Space", nullptr, windowFlags);
    ImGui::PopStyleVar(3);
    ImGui::DockSpace(ImGui::GetID("DockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

void GUISystem::drawMenuBar()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit"))
                glfwSetWindowShouldClose(this->_graphic->getWindow(), true);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem(ICON_FA_LIST " Scene Hierarchy", "Ctrl+1", &this->_showSceneHierarchy);
            ImGui::MenuItem(ICON_FA_INFO_CIRCLE " Properties", "Ctrl+2", &this->_showProperties);
            ImGui::MenuItem(ICON_FA_TOOLS " Tools", "Ctrl+3", &this->_showTools);
            ImGui::MenuItem(ICON_FA_TERMINAL " Console", "Ctrl+4", &this->_showConsole);
            ImGui::MenuItem(ICON_FA_EDIT " Scene", "Ctrl+5", &this->_showScene);
            ImGui::MenuItem(ICON_FA_GAMEPAD " Game", "Ctrl+6", &this->_showGame);
            ImGui::MenuItem(ICON_FA_FOLDER_OPEN " File Explorer", "Ctrl+7", &this->_showFileExplorer);
            ImGui::MenuItem(ICON_FA_STOPWATCH " Profiler", "Ctrl+8", &this->_showProfiler);
            ImGui::MenuItem(ICON_FA_SYNC " Reload Layout", nullptr, &this->_reloading);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void GUISystem::drawActionBar(EntityManager &entityManager, ComponentManager &componentManager)
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    if (ImGui::BeginViewportSideBar("ActionBar", ImGui::GetMainViewport(), ImGuiDir_Up, ImGui::GetFrameHeight(), window_flags)) {
        ImGui::PopStyleVar(1);
        if (ImGui::BeginMenuBar()) {
            ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x / 2 - 85, 0));
            ImGui::SameLine(0, 0);
            ImGui::PushStyleColor(ImGuiCol_Button, this->_graphic->getRunning() ? BUTTON_COLOR_SELECTED : BUTTON_COLOR);
            if (ImGui::Button(this->_graphic->getRunning() ? ICON_FA_STOP : ICON_FA_PLAY, ImVec2(50, 0))) {
                this->_graphic->setRunning(!this->_graphic->getRunning());
                if (this->_graphic->getRunning()) {
                    this->_entityManagerCopy = entityManager;
                    this->_componentManagerCopy = componentManager;
                    std::size_t it = 0;
                    for (auto &[name, script] : this->_resourceManager->getScripts()) {
                        for (auto id : entityManager.getMaskCategory(ComponentType::COMPONENT_TYPE_COUNT << it))
                            this->_resourceManager->getInstances().push_back(ScriptInstance(script, id));
                        it++;
                    }
                    for (auto &instance : this->_resourceManager->getInstances())
                        instance.init();
                } else {
                    this->_resourceManager->getInstances().clear();
                    entityManager = this->_entityManagerCopy;
                    componentManager = this->_componentManagerCopy;
                }
            }
            ImGui::PopStyleColor();
            ImGui::SameLine(0, 10);
            ImGui::PushStyleColor(ImGuiCol_Button, this->_graphic->getPaused() ? BUTTON_COLOR_SELECTED : BUTTON_COLOR);
            if (ImGui::Button(ICON_FA_PAUSE, ImVec2(50, 0)))
                this->_graphic->setPaused(!this->_graphic->getPaused());
            ImGui::PopStyleColor();
            ImGui::SameLine(0, 10);
            if (ImGui::Button(ICON_FA_STEP_FORWARD, ImVec2(50, 0)) && this->_graphic->getRunning() && this->_graphic->getPaused()) {
                this->_singleFrame = true;
                this->_graphic->setPaused(false);
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }
}

void GUISystem::drawSceneHierarchy(EntityManager &entityManager, ComponentManager &componentManager)
{
    if (!ImGui::Begin(ICON_FA_LIST " Scene Hierarchy", &this->_showSceneHierarchy)) {
        ImGui::End();
        return;
    }
    if (ImGui::BeginTable("Entities", 3, ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Mask");
        ImGui::TableSetupColumn("Actions");
        ImGui::TableHeadersRow();
        auto &masks = entityManager.getMasks();
        for (std::size_t i = 0; i < masks.size(); i++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%lu", i);
            ImGui::TableNextColumn();
            masks[i].has_value()
                ? ImGui::Text("%s", this->formatBinary(masks[i].value(), componentManager.getComponentArray().size() + this->_resourceManager->getScripts().size()).c_str())
                : ImGui::Text("None");
            ImGui::TableNextColumn();
            if (masks[i].has_value()) {
                if (ImGui::Button(std::string("Remove##" + std::to_string(i)).c_str())) {
                    entityManager.removeMask(i);
                    componentManager.removeAllComponents(i);
                }
                ImGui::SameLine();
                if (ImGui::Button(std::string("Modify##" + std::to_string(i)).c_str()))
                    this->_graphic->setSelectedEntity(i);
            } else {
                if (ImGui::Button(std::string("Create##" + std::to_string(i)).c_str()))
                    entityManager.addMask(i, 0);
            }
        }
        ImGui::EndTable();

        const float footerSize = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerSize), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::EndChild();
        ImGui::Separator();
        if (ImGui::Button("Add New Entity"))
            entityManager.addMask(masks.size(), std::nullopt);
    }
    ImGui::End();
}

const char *iconTable[] = {
    ICON_FA_INFO_CIRCLE " ", ICON_FA_RUNNING " ", ICON_FA_CUBE " ", ICON_FA_VIDEO " ", ICON_FA_LIGHTBULB " ", ICON_FA_MAP " ", ICON_FA_SIGN " ", ICON_FA_MAP " ",
};

void GUISystem::drawProperties(EntityManager &entityManager, ComponentManager &componentManager)
{
    if (!ImGui::Begin(ICON_FA_INFO_CIRCLE " Properties", &this->_showProperties)) {
        ImGui::End();
        return;
    }
    auto &masks = entityManager.getMasks();
    std::size_t selectedEntity = this->_graphic->getSelectedEntity();

    if (selectedEntity == (std::size_t)-1) {
        ImGui::Text("ID: None");
        ImGui::End();
        return;
    }
    ImGui::Text("ID: %lu", selectedEntity);
    if (!masks[selectedEntity].has_value()) {
        ImGui::Text("Entity does not exist");
        ImGui::End();
        return;
    }
    for (std::size_t i = 0; i < componentManager.getComponentArray().size(); i++) {
        if (masks[selectedEntity].value() & (1 << i)) {
            std::string name = FORMAT_NAME(componentManager.getComponentType(i).name());
            if (ImGui::CollapsingHeader((iconTable[i] + name).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                switch (i) {
                case 0: {
                    Transform &transform = componentManager.getComponent<Transform>(selectedEntity);
                    ImGui::DragFloat3("Position##Transform", (float *)&transform.position, 0.1f, -FLT_MAX, +FLT_MAX);
                    ImGui::DragFloat3("Rotation##Transform", (float *)&transform.rotation, 1.0f, -FLT_MAX, +FLT_MAX);
                    ImGui::DragFloat3("Scale##Transform", (float *)&transform.scale, 0.01f, 0.0f, +FLT_MAX);
                    break;
                }
                case 1: {
                    Velocity &velocity = componentManager.getComponent<Velocity>(selectedEntity);
                    ImGui::SliderFloat("X##Velocity", &velocity.x, -10, 10);
                    ImGui::SliderFloat("Y##Velocity", &velocity.y, -10, 10);
                    ImGui::SliderFloat("Z##Velocity", &velocity.z, -10, 10);
                    break;
                }
                case 2: {
                    Model &model = componentManager.getComponent<Model>(selectedEntity);
                    std::string selectedItem = model.name;
                    if (ImGui::BeginCombo("Select Item##Model", selectedItem.c_str())) {
                        for (auto &[key, value] : this->_resourceManager->getModels()) {
                            bool is_selected = (selectedItem == key);
                            if (ImGui::Selectable(key.c_str(), is_selected))
                                model.name = key;
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    break;
                }
                case 3: {
                    Camera &camera = componentManager.getComponent<Camera>(selectedEntity);
                    const char *names[CameraMod::CAMERA_MOD_COUNT] = {"First", "Third"};

                    ImGui::DragFloat3("Forward##Camera", (float *)&camera.forward, 0.01f, -1.0f, 1.0f);
                    ImGui::SliderInt("Mod##Camera", (int *)&camera.mod, 0, CameraMod::CAMERA_MOD_COUNT - 1, names[camera.mod]);
                    ImGui::SliderFloat("FOV##Camera", &camera.fov, 30.0f, 90.0f);
                    ImGui::SliderFloat("Near##Camera", &camera.near, 0.0f, 100.0f);
                    ImGui::SliderFloat("Far##Camera", &camera.far, 0.0f, 1000.0f);
                    ImGui::SliderFloat("Sensitivity##Camera", &camera.sensitivity, 0.0f, 1.0f);
                    if (camera.mod == CameraMod::THIRD_PERSON) {
                        ImGui::SliderFloat("Distance##Camera", &camera.distance, 0.0f, 100.0f);
                        const ImU64 increment = 1;
                        ImGui::InputScalar("Target ID##Camera", ImGuiDataType_U64, &camera.target, &increment);
                    }
                    break;
                }
                case 4: {
                    Light &light = componentManager.getComponent<Light>(selectedEntity);
                    const char *names[LightType::LIGHT_TYPE_COUNT] = {"Dir", "Point", "Spot", "Area"};

                    ImGui::SliderInt("Type##Light", (int *)&light.type, 0, LightType::LIGHT_TYPE_COUNT - 1, names[light.type]);
                    ImGui::SliderFloat("Intensity##Light", &light.intensity, 0.0f, 1.0f);
                    ImGui::ColorEdit3("Color##Light", (float *)&light.color);
                    ImGui::SliderFloat("Cut Off##Light", &light.cutOff, 0.0f, 90.0f);
                    break;
                }
                case 5: {
                    Cubemap &cubemap = componentManager.getComponent<Cubemap>(selectedEntity);
                    std::string selectedItem = cubemap.name;
                    if (ImGui::BeginCombo("Select Item##Cubemap", selectedItem.c_str())) {
                        for (auto &[key, value] : this->_resourceManager->getSkyboxes()) {
                            bool is_selected = (selectedItem == key);
                            if (ImGui::Selectable(key.c_str(), is_selected))
                                cubemap.name = key;
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    break;
                }
                case 6: {
                    Billboard &bill = componentManager.getComponent<Billboard>(selectedEntity);
                    std::string selectedItem = bill.name;
                    if (ImGui::BeginCombo("Select Item##Billboard", selectedItem.c_str())) {
                        for (auto &[key, value] : this->_resourceManager->getTextures()) {
                            bool is_selected = (selectedItem == key);
                            if (ImGui::Selectable(key.c_str(), is_selected))
                                bill.name = key;
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    break;
                }
                case 7: {
                    Map &map = componentManager.getComponent<Map>(selectedEntity);
                    std::string selectedItem = map.name;
                    if (ImGui::BeginCombo("Select Item##Map", selectedItem.c_str())) {
                        for (auto &[key, value] : this->_resourceManager->getTextures()) {
                            bool is_selected = (selectedItem == key);
                            if (ImGui::Selectable(key.c_str(), is_selected))
                                map.name = key;
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                    break;
                }
                default:
                    break;
                }
                // TODO: Clear this switch too
                if (ImGui::Button(std::string("Remove##" + std::to_string(i)).c_str())) {
                    entityManager.updateMask(selectedEntity, masks[selectedEntity].value() & ~(1 << i));
                    componentManager.removeSingleComponent(selectedEntity, componentManager.getComponentType(i));
                }
            }
            ImGui::Separator();
        }
    }
    std::size_t it = 0;
    for (auto &[name, script] : this->_resourceManager->getScripts()) {
        if (masks[selectedEntity].value() & (ComponentType::COMPONENT_TYPE_COUNT << it))
            if (ImGui::CollapsingHeader((ICON_FA_FILE_CODE " " + name).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                if (ImGui::Button(std::string("Remove##" + std::to_string(8 + it)).c_str()))
                    entityManager.updateMask(selectedEntity, masks[selectedEntity].value() & ~(ComponentType::COMPONENT_TYPE_COUNT << it));
        it++;
    }
    const float footerReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerReserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::EndChild();
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Add Component")) {
        for (std::size_t i = 0; i < componentManager.getComponentArray().size(); i++) {
            if (!(masks[selectedEntity].value() & (1 << i))) {
                ImGui::PushID(i);
                if (ImGui::Selectable("##selectable", false, ImGuiSelectableFlags_SpanAllColumns)) {
                    entityManager.updateMask(selectedEntity, masks[selectedEntity].value() | (1 << i));
                    switch (i) {
                    case 0:
                        componentManager.addComponent<Transform>(selectedEntity);
                        break;
                    case 1:
                        componentManager.addComponent<Velocity>(selectedEntity);
                        break;
                    case 2:
                        componentManager.addComponent<Model>(selectedEntity);
                        break;
                    case 3:
                        componentManager.addComponent<Camera>(selectedEntity);
                        break;
                    case 4:
                        componentManager.addComponent<Light>(selectedEntity);
                        break;
                    case 5:
                        componentManager.addComponent<Cubemap>(selectedEntity);
                        break;
                    case 6:
                        componentManager.addComponent<Billboard>(selectedEntity);
                        break;
                    case 7:
                        componentManager.addComponent<Map>(selectedEntity);
                        break;
                    default:
                        break;
                    }
                    // TODO: replace this switch
                }
                ImGui::SameLine(0, 0);
                ImGui::Text("%s", iconTable[i]);
                ImGui::SameLine(30, 0);
                ImGui::Text("%s", FORMAT_NAME(componentManager.getComponentType(i).name()));
                ImGui::PopID();
            }
        }
        it = 0;
        for (auto &[name, script] : this->_resourceManager->getScripts()) {
            if (!(masks[selectedEntity].value() & (ComponentType::COMPONENT_TYPE_COUNT << it))) {
                ImGui::PushID(8 + it);
                if (ImGui::Selectable("##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                    entityManager.updateMask(selectedEntity, masks[selectedEntity].value() | (ComponentType::COMPONENT_TYPE_COUNT << it));
                ImGui::SameLine(10, 0);
                ImGui::Text(ICON_FA_FILE_CODE);
                ImGui::SameLine(30, 0);
                ImGui::Text("%s", name.c_str());
                ImGui::PopID();
            }
            it++;
        }
    }
    ImGui::End();
}

void GUISystem::drawTools()
{
    if (!ImGui::Begin(ICON_FA_TOOLS " Tools", &this->_showTools)) {
        ImGui::End();
        return;
    }
    if (ImGui::RadioButton("Translate", this->_currentOperation == ImGuizmo::TRANSLATE))
        this->_currentOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", this->_currentOperation == ImGuizmo::ROTATE))
        this->_currentOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", this->_currentOperation == ImGuizmo::SCALE))
        this->_currentOperation = ImGuizmo::SCALE;
    ImGui::SameLine();
    if (this->_currentOperation != ImGuizmo::SCALE) {
        ImGui::Text("|");
        ImGui::SameLine();
        if (ImGui::RadioButton("Local", this->_currentMode == ImGuizmo::LOCAL))
            this->_currentMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("Global", this->_currentMode == ImGuizmo::WORLD))
            this->_currentMode = ImGuizmo::WORLD;
    } else
        this->_currentMode = ImGuizmo::LOCAL;
    ImGui::End();
}

void GUISystem::drawConsole()
{
    if (!ImGui::Begin(ICON_FA_TERMINAL " Console", &this->_showConsole)) {
        ImGui::End();
        return;
    }
    if (ImGui::SmallButton("Clear"))
        this->_consoleBuffer.clear();
    if (this->_graphic->getStringStream().str().size() > 0)
        this->_consoleBuffer += this->_graphic->getStringStream().str();
    ImGui::Separator();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::TextWrapped("%s", this->_consoleBuffer.c_str());
        this->_graphic->getStringStream().str("");
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();
    }
    ImGui::End();
}

void GUISystem::drawGame()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (!ImGui::Begin(ICON_FA_GAMEPAD " Game", &this->_showGame, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
        ImGui::PopStyleVar(3);
        ImGui::End();
        return;
    }
    ImGui::PopStyleVar(3);

    const float headerSize = ImGui::GetStyle().WindowPadding.y * 2.0f;
    this->_graphic->setGameViewPort({ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + headerSize, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y});

    GLuint texture = this->_graphic->getFrameBuffers()["render"].texture;
    ImGui::Image((void *)(intptr_t)texture, ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

void GUISystem::drawScene(EntityManager &entityManager, ComponentManager &componentManager)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (!ImGui::Begin(ICON_FA_EDIT " Scene", &this->_showScene, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
        ImGui::PopStyleVar(3);
        ImGui::End();
        return;
    }
    ImGui::PopStyleVar(3);

    bool guizmoDrawn = false;

    const float headerSize = ImGui::GetStyle().WindowPadding.y * 2.0f;
    this->_graphic->setSceneViewPort({ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + headerSize, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y});

    GLuint texture = this->_graphic->getFrameBuffers()["scene"].texture;
    ImGui::Image((void *)(intptr_t)texture, ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y), ImVec2(0, 1), ImVec2(1, 0));
    if (this->_showTools)
        guizmoDrawn = this->drawGuizmo(entityManager, componentManager);

    this->_graphic->setSceneMovement(ImGui::IsWindowHovered() && !(ImGuizmo::IsOver() && guizmoDrawn));

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("FILE")) {
            std::string file = (char *)payload->Data;
            if (file.substr(file.find_last_of(".") + 1) == "dae") {
                std::size_t id = entityManager.getMasks().size();
                entityManager.addMask(id, ComponentType::MODEL | ComponentType::TRANSFORM);
                componentManager.addComponent<Transform>(id);
                componentManager.addComponent<Model>(id, {file.substr(0, file.find_last_of("."))});
            }
            std::cout << "Dropped file: " << file << std::endl;
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::End();
}

bool GUISystem::drawGuizmo(EntityManager &entityManager, ComponentManager &componentManager)
{
    ImGuizmo::BeginFrame();

    if (!entityManager.hasMask(this->_graphic->getSelectedEntity(), ComponentType::TRANSFORM))
        return false;
    Transform &transform = componentManager.getComponent<Transform>(this->_graphic->getSelectedEntity());

    glm::vec4 viewport = this->_graphic->getSceneViewPort();
    glm::mat4 view = this->_graphic->getRenderView().getView();
    glm::mat4 projection = this->_graphic->getRenderView().getProjection();
    glm::mat4 model = glm::mat4(1.0f);

    float snapValue = this->_currentOperation == ImGuizmo::ROTATE ? 45.0f : 0.5f;
    float snap[3] = {snapValue, snapValue, snapValue};

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(viewport.x, viewport.y, viewport.z, viewport.w);
    ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(transform.position), glm::value_ptr(transform.rotation), glm::value_ptr(transform.scale), glm::value_ptr(model));
    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), this->_currentOperation, this->_currentMode, glm::value_ptr(model), nullptr,
                         ImGui::GetIO().KeyCtrl ? snap : nullptr);
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), glm::value_ptr(transform.position), glm::value_ptr(transform.rotation), glm::value_ptr(transform.scale));
    return true;
}

void GUISystem::drawFiles()
{
    if (!ImGui::Begin(ICON_FA_FOLDER_OPEN " File Explorer", &this->_showFileExplorer)) {
        ImGui::End();
        return;
    }
    int columns = (int)ImGui::GetContentRegionAvail().x / (50 + 20);
    if (columns < 1)
        columns = 1;

    if (ImGui::Button(ICON_FA_ARROW_LEFT " ..") && this->_fileExplorerPath != this->_fileExplorerRootPath)
        this->_fileExplorerPath = std::filesystem::path(this->_fileExplorerPath).parent_path().string();
    ImGui::SameLine();
    ImGui::Text("%s", this->_fileExplorerPath.c_str());

    ImGui::Columns(columns, nullptr, false);
    for (auto &file : std::filesystem::directory_iterator(this->_fileExplorerPath)) {
        ImGui::SetWindowFontScale(2.0f);
        if (file.is_directory()) {
            ImGui::Button((ICON_FA_FOLDER "##" + file.path().string()).c_str(), ImVec2(50, 50));
            if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
                this->_fileExplorerPath = file.path().string();
        } else {
            ImGui::Button((ICON_FA_FILE "##" + file.path().string()).c_str(), ImVec2(50, 50));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("FILE", file.path().filename().string().c_str(), file.path().filename().string().size() + 1);
                ImGui::Text("%s", file.path().filename().string().c_str());
                ImGui::EndDragDropSource();
            }
            if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
                std::cout << "Opening file: " << file.path().string() << std::endl;
        }
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("%s", file.path().filename().string().c_str());
        ImGui::NextColumn();
    }
    ImGui::Columns(1); // reset columns
    ImGui::End();
}

void GUISystem::drawProfiler()
{
    if (!ImGui::Begin(ICON_FA_STOPWATCH " Profiler", &this->_showProfiler)) {
        ImGui::End();
        return;
    }
    ImGui::End();
}

std::string GUISystem::formatBinary(std::size_t value, std::size_t size)
{
    std::string binary = std::bitset<64>(value).to_string();
    std::string result = "";

    for (std::size_t i = 64 - size; i < 64; i++) {
        result += binary[i];
        if (i % 8 == 7)
            result += " ";
    }
    return result;
}
