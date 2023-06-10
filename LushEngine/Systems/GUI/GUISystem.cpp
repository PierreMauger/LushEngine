#include "Systems/GUI/GUISystem.hpp"

using namespace Lush;

static const char *lightTypeNames[LightType::LIGHT_TYPE_COUNT] = {"Dir", "Point", "Spot", "Area"};

GUISystem::GUISystem(std::shared_ptr<Graphic> graphic, std::shared_ptr<ResourceManager> resourceManager)
    : ASystem(60.0f), _graphic(std::move(graphic)), _resourceManager(std::move(resourceManager))
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

    static const ImWchar iconsRanges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig iconsConfig;
    iconsConfig.MergeMode = true;
    iconsConfig.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("Resources/Fonts/" FONT_ICON_FILE_NAME_FAS, 12.0f, &iconsConfig, iconsRanges);

    ImGuizmo::Enable(true);
    ImGuizmo::AllowAxisFlip(false);

    this->_fileBrowserPath = std::filesystem::current_path().string();
}

GUISystem::~GUISystem()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUISystem::update(EntityManager &entityManager, float deltaTime)
{
    if (!this->shouldUpdate(deltaTime))
        return;
    if (this->_singleFrame) {
        this->_singleFrame = false;
        this->_graphic->setPaused(true);
    }
    this->handleConsole();
    UpdateToasts(deltaTime);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    setDock();
    this->drawMenuBar();
    this->drawActionBar(entityManager);

    if (this->_showSceneHierarchy)
        this->drawSceneHierarchy(entityManager);
    if (this->_showProperties)
        this->drawProperties(entityManager);
    if (this->_showTools)
        this->drawTools();
    if (this->_showConsole)
        this->drawConsole();
    if (this->_showGame)
        this->drawGame();
    if (this->_showScene)
        this->drawScene(entityManager);
    if (this->_showFileExplorer)
        this->drawFiles();
    if (this->_showProfiler)
        this->drawProfiler();
    if (this->_showFileBrowser)
        this->drawFileBrowser();
    DrawToasts();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (this->_resetLayout) {
        ImGui::LoadIniSettingsFromDisk("Resources/Config/dockConfig.ini");
        this->_resetLayout = false;
        this->_showSceneHierarchy = true;
        this->_showProperties = true;
        this->_showTools = true;
        this->_showConsole = true;
        this->_showScene = true;
        this->_showGame = true;
        this->_showFileExplorer = true;
        this->_showProfiler = true;
        this->_showFileBrowser = false;
    }
}

void GUISystem::handleConsole()
{
    if (!this->_graphic->getStringStream().str().empty()) {
        std::istringstream iss(this->_graphic->getStringStream().str());
        for (std::string line; std::getline(iss, line);) {
            if (line.substr(0, 13) == "[Toast Error]")
                CreateToast(ICON_FA_STOP_CIRCLE " Error", line.substr(13), ToastType::ERROR);
            else if (line.substr(0, 15) == "[Toast Warning]")
                CreateToast(ICON_FA_EXCLAMATION_TRIANGLE " Warning", line.substr(15), ToastType::WARNING);
            else if (line.substr(0, 12) == "[Toast Info]")
                CreateToast(ICON_FA_INFO_CIRCLE " Info", line.substr(12), ToastType::INFO);
            else if (line.substr(0, 15) == "[Toast Success]")
                CreateToast(ICON_FA_CHECK_CIRCLE " Success", line.substr(15), ToastType::SUCCESS);

            std::time_t t = std::time(nullptr);
            std::tm tm = *std::localtime(&t);
            std::stringstream ss;
            ss << std::put_time(&tm, "[%H:%M:%S] ");

            this->_consoleBuffer += ss.str() + line + '\n';
        }
        this->_graphic->getStringStream().str("");
    }
}

void GUISystem::setDock()
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos({viewport->Pos.x, viewport->Pos.y + 19});
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
            if (ImGui::MenuItem("Open..."))
                this->_showFileBrowser = true;
            if (ImGui::MenuItem("Build"))
                this->build();
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
            ImGui::MenuItem(ICON_FA_SYNC " Reset Layout", nullptr, &this->_resetLayout);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem(ICON_FA_BORDER_ALL " Wireframe"))
                this->_graphic->setWireframe(!this->_graphic->isWireframe());
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void GUISystem::drawActionBar(EntityManager &entityManager)
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
                    std::size_t it = this->_resourceManager->getScripts().size() - 1;
                    for (auto &[name, script] : this->_resourceManager->getScripts()) {
                        for (auto &[id, entity] : entityManager.getEntities())
                            if (entity.hasScriptComponent(name))
                                this->_resourceManager->getInstances().emplace_back(script, id, entity.getScriptComponent(name).getFields());
                        it--;
                    }
                    for (auto &instance : this->_resourceManager->getInstances())
                        instance.init();
                } else {
                    this->_resourceManager->getInstances().clear();
                    entityManager = this->_entityManagerCopy;
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

void GUISystem::drawSceneHierarchy(EntityManager &entityManager)
{
    if (!ImGui::Begin(ICON_FA_LIST " Scene Hierarchy", &this->_showSceneHierarchy)) {
        ImGui::End();
        return;
    }
    if (ImGui::CollapsingHeader("Scenes")) {
        for (auto &[name, scene] : this->_resourceManager->getScenes()) {
            if (name != this->_resourceManager->getActiveScene() && ImGui::Selectable(name.c_str(), this->_resourceManager->getActiveScene() == name)) {
                this->_resourceManager->setActiveScene(name);
                scene.setScene(entityManager);
            }
        }
    }
    if (this->_resourceManager->getActiveScene().empty()) {
        ImGui::End();
        return;
    }
    if (ImGui::BeginTable("Entities", 3, ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 30.0f);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("##Actions", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 20.0f);
        ImGui::TableHeadersRow();

        for (auto &[id, entity] : entityManager.getEntities()) {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::PushID(id);
            ImGui::Selectable("##Select", id == this->_graphic->getSelectedEntity(), ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0, 20));
            if (ImGui::IsItemClicked())
                this->_graphic->setSelectedEntity(id);
            ImGui::SameLine();
            ImGui::Text("%lu", id);
            ImGui::TableNextColumn();
            char buf[256];
            snprintf(buf, 256, "%s", entity.getName().c_str());
            if (ImGui::InputText("##Name", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
                entity.setName(buf);
                std::cout << "Entity renamed to " << entity.getName() << std::endl;
            }
            ImGui::TableNextColumn();

            if (ImGui::Button(ICON_FA_TRASH "##Delete")) {
                entityManager.removeEntity(id);
                ImGui::PopID();
                break;
            }
            ImGui::PopID();
        }
        ImGui::EndTable();

        const float footerSize = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerSize), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::EndChild();
        ImGui::Separator();
        if (ImGui::Button("Add New Entity")) {
            Entity entity;
            entityManager.addEntity(entity);
        }
    }
    ImGui::End();
}

void GUISystem::drawProperties(EntityManager &entityManager)
{
    if (!ImGui::Begin(ICON_FA_INFO_CIRCLE " Properties", &this->_showProperties)) {
        ImGui::End();
        return;
    }

    if (!entityManager.hasEntity(this->_graphic->getSelectedEntity())) {
        ImGui::Text("No entity selected");
        ImGui::End();
        return;
    }
    ImGui::Text("ID: %lu", this->_graphic->getSelectedEntity());
    Entity &entity = entityManager.getEntity(this->_graphic->getSelectedEntity());
    if (entity.hasComponent<Transform>()) {
        if (ImGui::CollapsingHeader(ICON_FA_INFO_CIRCLE " Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            Transform &transform = entity.getComponent<Transform>();

            ImGui::DragFloat3("Position##Transform", (float *)&transform.position, 0.1f, -FLT_MAX, +FLT_MAX);
            ImGui::DragFloat3("Rotation##Transform", (float *)&transform.rotation, 1.0f, -FLT_MAX, +FLT_MAX);
            ImGui::DragFloat3("Scale##Transform", (float *)&transform.scale, 0.01f, 0.0f, +FLT_MAX);
            if (ImGui::Button("Remove##Transform"))
                entity.removeComponent<Transform>();
            ImGui::Separator();
        }
    }
    if (entity.hasComponent<Model>()) {
        if (ImGui::CollapsingHeader(ICON_FA_CUBE " Model", ImGuiTreeNodeFlags_DefaultOpen)) {
            Model &model = entity.getComponent<Model>();

            std::string selectedItem = model.name;
            if (ImGui::BeginCombo("Select Item##Model", selectedItem.c_str())) {
                for (auto &[key, value] : this->_resourceManager->getModels()) {
                    bool isSelected = (selectedItem == key);
                    if (ImGui::Selectable(key.c_str(), isSelected))
                        model.name = key;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("Remove##Model"))
                entity.removeComponent<Model>();
            ImGui::Separator();
        }
    }
    if (entity.hasComponent<Camera>()) {
        if (ImGui::CollapsingHeader(ICON_FA_VIDEO " Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            Camera &camera = entity.getComponent<Camera>();

            ImGui::DragFloat3("Forward##Camera", (float *)&camera.forward, 0.01f, -1.0f, 1.0f);
            ImGui::SliderFloat("FOV##Camera", &camera.fov, 30.0f, 90.0f);
            ImGui::SliderFloat("Near##Camera", &camera.near, 0.1f, 100.0f);
            ImGui::SliderFloat("Far##Camera", &camera.far, camera.near + 0.1f, 1000.0f);
            if (ImGui::Button("Remove##Camera"))
                entity.removeComponent<Camera>();
            ImGui::Separator();
        }
    }
    if (entity.hasComponent<Light>()) {
        if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB " Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            Light &light = entity.getComponent<Light>();

            ImGui::SliderInt("Type##Light", (int *)&light.type, 0, LightType::LIGHT_TYPE_COUNT - 1, lightTypeNames[light.type]);
            ImGui::SliderFloat("Intensity##Light", &light.intensity, 0.0f, 1.0f);
            ImGui::ColorEdit3("Color##Light", (float *)&light.color);
            ImGui::SliderFloat("Cut Off##Light", &light.cutOff, 0.0f, 90.0f);
            if (ImGui::Button("Remove##Light"))
                entity.removeComponent<Light>();
            ImGui::Separator();
        }
    }
    if (entity.hasComponent<Cubemap>()) {
        if (ImGui::CollapsingHeader(ICON_FA_MAP " Cubemap", ImGuiTreeNodeFlags_DefaultOpen)) {
            Cubemap &cubemap = entity.getComponent<Cubemap>();

            std::string selectedItem = cubemap.name;
            if (ImGui::BeginCombo("Select Item##Cubemap", selectedItem.c_str())) {
                for (auto &[key, value] : this->_resourceManager->getSkyBoxes()) {
                    bool isSelected = (selectedItem == key);
                    if (ImGui::Selectable(key.c_str(), isSelected))
                        cubemap.name = key;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("Remove##Cubemap"))
                entity.removeComponent<Cubemap>();
            ImGui::Separator();
        }
    }
    if (entity.hasComponent<Billboard>()) {
        if (ImGui::CollapsingHeader(ICON_FA_SIGN " Billboard", ImGuiTreeNodeFlags_DefaultOpen)) {
            Billboard &billboard = entity.getComponent<Billboard>();

            this->drawTextureSelect("Texture##Billboard", billboard.name);
            if (ImGui::Button("Remove##Billboard"))
                entity.removeComponent<Billboard>();
            ImGui::Separator();
        }
    }
    if (entity.hasComponent<Map>()) {
        if (ImGui::CollapsingHeader(ICON_FA_MAP " Map", ImGuiTreeNodeFlags_DefaultOpen)) {
            Map &map = entity.getComponent<Map>();

            this->drawTextureSelect("Height Map##Map", map.heightMap);
            this->drawTextureSelect("Diffuse Texture##Map", map.diffuseTexture);
            this->drawTextureSelect("Diffuse Texture 2##Map", map.diffuseTexture2);
            this->drawTextureSelect("Diffuse Texture 3##Map", map.diffuseTexture3);

            if (ImGui::Button("Remove##Map"))
                entity.removeComponent<Map>();
            ImGui::Separator();
        }
    }
    std::size_t it = 0;
    for (auto &[scriptName, script] : this->_resourceManager->getScripts()) {
        if (entity.hasScriptComponent(scriptName)) {
            std::size_t instance = this->getScriptInstanceIndex(this->_graphic->getSelectedEntity());
            if (ImGui::CollapsingHeader((ICON_FA_FILE_CODE " " + scriptName).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                for (auto &[fieldName, field] : script.getFields()) {
                    if (instance != (std::size_t)-1) {
                        if (field.type == "Single") {
                            float value = this->_resourceManager->getInstances()[instance].getFieldValue<float>(fieldName);
                            if (ImGui::DragFloat(fieldName.c_str(), &value))
                                this->_resourceManager->getInstances()[instance].setFieldValue(fieldName, value);
                        }
                        if (field.type == "Entity" || field.type == "UInt64") {
                            unsigned long value = this->_resourceManager->getInstances()[instance].getFieldValue<unsigned long>(fieldName);
                            const ImU64 increment = 1;
                            if (ImGui::InputScalar(fieldName.c_str(), ImGuiDataType_U64, &value, &increment))
                                this->_resourceManager->getInstances()[instance].setFieldValue(fieldName, value);
                        }
                    } else {
                        if (field.type == "Single") {
                            float &value = entity.getScriptComponent(scriptName).getField<float>(fieldName);
                            ImGui::DragFloat(fieldName.c_str(), &value);
                        }
                        if (field.type == "Entity" || field.type == "UInt64") {
                            unsigned long &value = entity.getScriptComponent(scriptName).getField<unsigned long>(fieldName);
                            const ImU64 increment = 1;
                            ImGui::InputScalar(fieldName.c_str(), ImGuiDataType_U64, &value, &increment);
                        }
                    }
                }
                if (ImGui::Button(("Remove##" + std::to_string(it)).c_str()))
                    entity.removeScriptComponent(scriptName);
                ImGui::Separator();
            }
            it++;
        }
    }

    const float footerReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerReserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::EndChild();
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Add Component")) {
        if (!entity.hasComponent<Transform>()) {
            ImGui::Text("%s", ICON_FA_ARROWS_ALT);
            ImGui::SameLine(30, 0);
            if (ImGui::Selectable("Transform##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Transform());
        }
        if (!entity.hasComponent<Model>()) {
            ImGui::Text("%s", ICON_FA_CUBE);
            ImGui::SameLine(30, 0);
            if (ImGui::Selectable("Model##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Model());
        }
        if (!entity.hasComponent<Camera>()) {
            ImGui::Text("%s", ICON_FA_CAMERA);
            ImGui::SameLine(30, 0);
            if (ImGui::Selectable("Camera##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Camera());
        }
        if (!entity.hasComponent<Light>()) {
            ImGui::Text("%s", ICON_FA_LIGHTBULB);
            ImGui::SameLine(30, 0);
            if (ImGui::Selectable("Light##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Light());
        }
        if (!entity.hasComponent<Cubemap>()) {
            ImGui::Text("%s", ICON_FA_CUBE);
            ImGui::SameLine(30, 0);
            if (ImGui::Selectable("Cubemap##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Cubemap());
        }
        if (!entity.hasComponent<Billboard>()) {
            ImGui::Text("%s", ICON_FA_SIGN);
            ImGui::SameLine(30, 0);
            if (ImGui::Selectable("Billboard##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Billboard());
        }
        if (!entity.hasComponent<Map>()) {
            ImGui::Text("%s", ICON_FA_MAP);
            ImGui::SameLine(30, 0);
            if (ImGui::Selectable("Map##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Map());
        }

        it = 0;
        for (auto &[scriptName, script] : this->_resourceManager->getScripts()) {
            if (entity.hasScriptComponent(scriptName))
                continue;
            ImGui::PushID((int)it);
            ImGui::Text(ICON_FA_FILE_CODE);
            ImGui::SameLine(30, 0);
            if (ImGui::Selectable((scriptName + "##selectable").c_str(), false, ImGuiSelectableFlags_SpanAllColumns)) {
                ScriptComponent scriptComponent;
                for (auto &[fieldName, field] : script.getFields()) {
                    if (field.type == "Single")
                        scriptComponent.addField(fieldName, 0.0f);
                    if (field.type == "Entity" || field.type == "UInt64")
                        scriptComponent.addField(fieldName, (unsigned long)0);
                }
                entity.addScriptComponent(scriptName, scriptComponent);
            }
            ImGui::PopID();
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
    ImGui::Separator();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::TextWrapped("%s", this->_consoleBuffer.c_str());
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

void GUISystem::drawScene(EntityManager &entityManager)
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

    const float headerSize = ImGui::GetStyle().WindowPadding.y * 2.0f;
    this->_graphic->setSceneViewPort({ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + headerSize, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y});

    GLuint texture = this->_graphic->getFrameBuffers()["scene"].texture;
    ImGui::Image((void *)(intptr_t)texture, ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y), ImVec2(0, 1), ImVec2(1, 0));

    bool guizmoDrawn = false;
    if (this->_showTools && entityManager.hasEntity(this->_graphic->getSelectedEntity()))
        guizmoDrawn = this->drawGuizmo(entityManager);

    this->_graphic->setSceneMovement(ImGui::IsWindowHovered() && !(ImGuizmo::IsOver() && guizmoDrawn));

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("FILE")) {
            std::string file = (char *)payload->Data;
            if (file.substr(file.find_last_of('.') + 1) == "dae") {
                Entity entity;
                entity.addComponent(Transform());
                Model model;
                model.name = file.substr(0, file.find_last_of('.'));
                entity.addComponent(model);
                entityManager.addEntity(entity);
            }
            std::cout << "[Toast Info]Dropped file: " << file << std::endl;
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::End();
}

bool GUISystem::drawGuizmo(EntityManager &entityManager)
{
    ImGuizmo::BeginFrame();

    Entity &selectedEntity = entityManager.getEntity(this->_graphic->getSelectedEntity());
    if (!selectedEntity.hasComponent<Transform>())
        return false;
    Transform &transform = selectedEntity.getComponent<Transform>();

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
    return ImGuizmo::IsUsing();
}

void GUISystem::drawFiles()
{
    if (!ImGui::Begin(ICON_FA_FOLDER_OPEN " File Explorer", &this->_showFileExplorer)) {
        ImGui::End();
        return;
    }
    if (this->_projectPath.empty()) {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("No scene loaded").x) / 2);
        ImGui::SetCursorPosY((ImGui::GetWindowHeight() - ImGui::CalcTextSize("No scene loaded").y) / 2 - 20);
        ImGui::Text("No project loaded");

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 20);
        if (ImGui::Button(ICON_FA_PLUS, ImVec2(40, 40))) {
            this->_showFileBrowser = true;
        }
        ImGui::End();
        return;
    }

    int columns = (int)ImGui::GetContentRegionAvail().x / (50 + 20);
    if (columns < 1)
        columns = 1;

    if (ImGui::Button(ICON_FA_ARROW_LEFT " ..") && this->_projectPath != this->_projectRootPath)
        this->_projectPath = std::filesystem::path(this->_projectPath).parent_path().string();
    ImGui::SameLine();
    ImGui::Text("%s", this->_projectPath.c_str());

    ImGui::Columns(columns, nullptr, false);
    for (auto &file : std::filesystem::directory_iterator(this->_projectPath)) {
        ImGui::SetWindowFontScale(2.0f);
        if (file.is_directory()) {
            ImGui::Button((ICON_FA_FOLDER "##" + file.path().string()).c_str(), ImVec2(50, 50));
            if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
                this->_projectPath = file.path().string();
        } else {
            ImGui::Button((ICON_FA_FILE "##" + file.path().string()).c_str(), ImVec2(50, 50));
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                ImGui::SetDragDropPayload("FILE", file.path().filename().string().c_str(), file.path().filename().string().size() + 1);
                ImGui::Text("%s", file.path().filename().string().c_str());
                ImGui::EndDragDropSource();
            }
            if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
                std::cout << "[Toast Info]Opening file: " << file.path().string() << std::endl;
        }
        ImGui::SetWindowFontScale(1.0f);
        ImGui::Text("%s", file.path().filename().string().c_str());
        ImGui::NextColumn();
    }
    ImGui::Columns(1);
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

void GUISystem::drawFileBrowser()
{
    ImGuiWindowClass windowClass;
    windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoDockingSplitOther;
    ImGui::SetNextWindowClass(&windowClass);
    if (!ImGui::Begin("Open Project", &this->_showFileBrowser)) {
        ImGui::End();
        return;
    }
    if (ImGui::Button(ICON_FA_ARROW_LEFT " .."))
        this->_fileBrowserPath = std::filesystem::path(this->_fileBrowserPath).parent_path().string();
    ImGui::SameLine();
    ImGui::Text("%s", this->_fileBrowserPath.c_str());
    for (const auto &entry : std::filesystem::directory_iterator(this->_fileBrowserPath)) {
        const auto &path = entry.path();
        const auto &filenameStr = path.filename().string();
        if (entry.is_directory() && filenameStr[0] != '.') {
            if (ImGui::Selectable(filenameStr.c_str(), false, ImGuiSelectableFlags_DontClosePopups)) {
                this->_fileBrowserPath = path.string();
            }
        }
    }
    const float footerReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerReserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::EndChild();
    ImGui::Separator();
    if (ImGui::Button("Cancel"))
        this->_showFileBrowser = false;
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Open").x - ImGui::GetStyle().ItemSpacing.x * 2, 0));
    ImGui::SameLine();
    if (ImGui::Button("Open")) {
        this->_projectRootPath = this->_fileBrowserPath;
        this->_projectPath = this->_projectRootPath;
        glfwSetWindowTitle(this->_graphic->getWindow(), std::string("Lush Engine - " + std::filesystem::path(this->_projectRootPath).filename().string()).c_str());
        this->_showFileBrowser = false;
        this->_resourceManager->loadProject(this->_projectRootPath);
    }
    ImGui::End();
}

std::string GUISystem::formatBinary(std::size_t value, std::size_t size)
{
    std::string binary = std::bitset<64>(value).to_string();
    std::string result;

    for (std::size_t i = 64 - size; i < 64; i++) {
        result += binary[i];
        if (i % 8 == 7)
            result += " ";
    }
    return result;
}

std::size_t GUISystem::getScriptInstanceIndex(std::size_t entityId)
{
    std::size_t i = 0;

    for (auto &instance : this->_resourceManager->getInstances()) {
        if (instance.getId() == entityId)
            return i;
        i++;
    }
    return (std::size_t)-1;
}

void GUISystem::drawTextureSelect(const std::string &fieldName, std::string &texture)
{
    std::string selectedItem = texture;
    if (ImGui::BeginCombo(fieldName.c_str(), selectedItem.c_str())) {
        for (auto &[key, value] : this->_resourceManager->getTextures()) {
            bool isSelected = (selectedItem == key);
            if (ImGui::Selectable(key.c_str(), isSelected))
                texture = key;
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        if (ImGui::Selectable("None", selectedItem == "None"))
            texture = "None";
        ImGui::EndCombo();
    }
}

void GUISystem::build()
{
    if (this->_projectRootPath.empty()) {
        std::cout << "[Toast Warning]No project opened" << std::endl;
        return;
    }
    std::filesystem::create_directories(this->_projectRootPath + "/Resources/bin");

    this->_resourceManager->serializeAssetPack();
    std::filesystem::copy_file("lush", std::filesystem::path(this->_projectRootPath) / (std::filesystem::path(this->_projectRootPath).filename().string()),
                               std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file("Resources/AssetPack.data", std::filesystem::path(this->_projectRootPath) / "Resources" / "AssetPack.data",
                               std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy("Resources/bin", std::filesystem::path(this->_projectRootPath) / "Resources" / "bin",
                          std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
}
