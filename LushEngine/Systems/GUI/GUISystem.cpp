#include "Systems/GUI/GUISystem.hpp"
#include <utility>

using namespace Lush;

static const char *cameraTypeNames[CameraType::CAMERA_TYPE_COUNT] = {"Perspective", "Orthographic"};
static const char *lightTypeNames[LightType::LIGHT_TYPE_COUNT] = {"Dir", "Point", "Spot", "Area"};
static const char *colliderTypeNames[ColliderType::COLLIDER_TYPE_COUNT] = {"box", "Sphere", "Capsule", "Mesh"};

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
    this->_fileExplorerPath = std::filesystem::current_path().string();

    this->loadProjectSettings();
    Shapes::setupPlane(this->_screen);
}

GUISystem::~GUISystem()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUISystem::loadProjectSettings()
{
    std::ifstream ifs("Resources/Config/projectSettings.txt", std::ios::binary);
    boost::archive::binary_iarchive ia(ifs, boost::archive::no_header);

    if (ifs.peek() == std::ifstream::traits_type::eof()) {
        ifs.close();
        return;
    }
    ia >> this->_projectSettings;

    ifs.close();
}

void GUISystem::saveProjectSettings()
{
    std::ofstream ofs("Resources/Config/projectSettings.txt", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs, boost::archive::no_header);

    oa << this->_projectSettings;

    ofs.close();
}

void GUISystem::update(std::shared_ptr<EntityManager> &entityManager, float deltaTime)
{
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

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S) && ImGui::GetIO().KeyCtrl) {
        this->_resourceManager->getActiveScene().save(this->_resourceManager->getScripts());
        std::cout << "[Toast Success]Scene " << this->_resourceManager->getActiveScene().getFiles()[0].getName() << " saved" << std::endl;
    }

    if (this->_showSceneHierarchy)
        this->drawSceneHierarchy(entityManager);
    if (this->_showProperties)
        this->drawProperties(entityManager);
    if (this->_showConsole)
        this->drawConsole();
    if (this->_showGame)
        this->drawGame();
    if (this->_showScene)
        this->drawScene(entityManager);
    if (this->_showFileExplorer)
        this->drawFileExplorer();
    if (this->_showProfiler)
        this->drawProfiler();
    if (this->_showProjectManager)
        this->drawProjectManager(entityManager);
    if (this->_showAbout)
        this->drawAbout();

    if (ImGui::Begin("Debug shadows")) {
        GLuint texture = this->_graphic->getFrameBuffers()["shadow"].texture;
        float smallest = std::min(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
        ImGui::Image((void *)(intptr_t)texture, ImVec2(smallest, smallest), ImVec2(1, 0), ImVec2(0, 1));
        ImGui::End();
    }

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
        this->_showProjectManager = false;
        this->_showAbout = false;
    }
}

void GUISystem::handleConsole()
{
    if (!this->_graphic->getStringStream().str().empty()) {
        std::istringstream iss(this->_graphic->getStringStream().str());
        for (std::string line; std::getline(iss, line);) {
            int type = ToastType::NONE;
            if (line.substr(0, 13) == "[Toast Error]") {
                line = line.substr(13);
                CreateToast(ICON_FA_STOP_CIRCLE " Error", line, ToastType::ERROR);
                type = ToastType::ERROR;
            } else if (line.substr(0, 15) == "[Toast Warning]") {
                line = line.substr(15);
                CreateToast(ICON_FA_EXCLAMATION_TRIANGLE " Warning", line, ToastType::WARNING);
                type = ToastType::WARNING;
            } else if (line.substr(0, 12) == "[Toast Info]") {
                line = line.substr(12);
                CreateToast(ICON_FA_INFO_CIRCLE " Info", line, ToastType::INFO);
                type = ToastType::INFO;
            } else if (line.substr(0, 15) == "[Toast Success]") {
                line = line.substr(15);
                CreateToast(ICON_FA_CHECK_CIRCLE " Success", line, ToastType::SUCCESS);
                type = ToastType::SUCCESS;
            }

            std::time_t t = std::time(nullptr);
            std::tm tm = *std::localtime(&t);
            std::stringstream ss;
            ss << std::put_time(&tm, "[%H:%M:%S] ");

            this->_consoleBuffer.emplace_back(type, ss.str() + line);
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
            if (ImGui::MenuItem("Exit"))
                glfwSetWindowShouldClose(this->_graphic->getWindow(), true);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Project")) {
            if (ImGui::MenuItem("Project Manager")) {
                this->_showProjectManager = true;
                this->_editingProject.clear();
            }
            if (ImGui::MenuItem("Build"))
                this->build();
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
            if (ImGui::MenuItem(ICON_FA_BORDER_ALL " Wireframe", nullptr, this->_graphic->isWireframe()))
                this->_graphic->setWireframe(!this->_graphic->isWireframe());
            if (ImGui::MenuItem(ICON_FA_EYE " Post Processing", nullptr, this->_graphic->isPostProcessing()))
                this->_graphic->setPostProcessing(!this->_graphic->isPostProcessing());
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About"))
                this->_showAbout = true;
            if (ImGui::MenuItem("Open Documentation")) {
                std::cout << "[Toast Info]Opening documentation in browser" << std::endl;
                std::system("xdg-open " DOCUMENTATION_URL);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GUISystem::drawActionBar(std::shared_ptr<EntityManager> &entityManager)
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    if (ImGui::BeginViewportSideBar("ActionBar", ImGui::GetMainViewport(), ImGuiDir_Up, ImGui::GetFrameHeight(), window_flags)) {
        ImGui::PopStyleVar(1);
        if (ImGui::BeginMenuBar()) {
            ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x / 2 - 85, 0));
            ImGui::SameLine(0, 0);
            ImGui::PushStyleColor(ImGuiCol_Button, this->_graphic->isRunning() ? BUTTON_COLOR_SELECTED : BUTTON_COLOR);
            if (ImGui::Button(this->_graphic->isRunning() ? ICON_FA_STOP : ICON_FA_PLAY, ImVec2(50, 0))) {
                this->_graphic->setRunning(!this->_graphic->isRunning());
                if (this->_graphic->isRunning()) {
                    entityManager = std::make_shared<EntityManager>();
                    entityManager->clone(*this->_resourceManager->getActiveScene().getEntityManager());
                    this->_resourceManager->initScriptInstances(entityManager);
                    this->_resourceManager->initPhysicInstances(entityManager);
                } else {
                    this->_resourceManager->getScriptInstances().clear();
                    this->_resourceManager->resetDynamicsWorld();
                    this->_resourceManager->getPhysicInstances().clear();
                    entityManager = this->_resourceManager->getActiveScene().getEntityManager();
                }
            }
            ImGui::PopStyleColor();
            ImGui::SameLine(0, 10);
            ImGui::PushStyleColor(ImGuiCol_Button, this->_graphic->isPaused() ? BUTTON_COLOR_SELECTED : BUTTON_COLOR);
            if (ImGui::Button(ICON_FA_PAUSE, ImVec2(50, 0)))
                this->_graphic->setPaused(!this->_graphic->isPaused());
            ImGui::PopStyleColor();
            ImGui::SameLine(0, 10);
            if (ImGui::Button(ICON_FA_STEP_FORWARD, ImVec2(50, 0)) && this->_graphic->isRunning() && this->_graphic->isPaused()) {
                this->_singleFrame = true;
                this->_graphic->setPaused(false);
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }
}

void GUISystem::deleteEntity(std::shared_ptr<EntityManager> &entityManager, std::size_t id, Entity &entity)
{
    for (auto &childId : entity.getChildren())
        this->deleteEntity(entityManager, childId, entityManager->getEntity(childId));
    if (entity.getParent().has_value())
        entityManager->getEntity(entity.getParent().value()).removeChild(id);
    entityManager->removeEntity(id);
}

bool GUISystem::drawEntityInSceneHierarchy(std::shared_ptr<EntityManager> &entityManager, std::size_t id, Entity &entity)
{
    ImGui::PushID(entity.getUUID().toString().c_str());
    ImGui::Selectable("", id == this->_graphic->getSelectedEntity(), ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0, 19));

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("ENTITY", &id, sizeof(int));
        ImGui::Text("%s", entity.getName().c_str());
        ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY")) {
            // dragged entity is the child
            int draggedEntityId = *(const int *)payload->Data;
            if (entityManager->getEntities().contains(draggedEntityId)) {
                Entity &draggedEntity = entityManager->getEntity(draggedEntityId);
                // if not dragging parent into child or child into parent
                if (!(draggedEntity.getParent().has_value() && draggedEntity.getParent().value() == id) &&
                    !(entity.getParent().has_value() && entity.getParent().value() == draggedEntityId)) {
                    if (draggedEntity.getParent().has_value())
                        entityManager->getEntity(draggedEntity.getParent().value()).removeChild(draggedEntityId);
                    draggedEntity.setParent(id);
                    entity.addChild(draggedEntityId);
                    if (draggedEntity.hasComponent<Transform>() && entity.hasComponent<Transform>()) {
                        Transform &draggedTransform = draggedEntity.getComponent<Transform>();
                        Transform &entityTransform = entity.getComponent<Transform>();
                        draggedTransform.rotation = entityTransform.rotation * draggedTransform.rotation;
                        draggedTransform.position = entityTransform.rotation * draggedTransform.position + entityTransform.position;
                    }
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemClicked())
        this->_graphic->setSelectedEntity(id);

    ImGui::SameLine(0, 0);
    char buf[256];
    snprintf(buf, 256, "%s", entity.getName().c_str());
    if (ImGui::InputText("##Name", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
        entity.setName(buf);
        std::cout << "[Toast Info]Entity renamed to " << entity.getName() << std::endl;
    }
    ImGui::SameLine(0, 0);
    ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - 24, 0));
    ImGui::SameLine(0, 0);
    if (ImGui::Button(ICON_FA_TRASH "##Delete")) {
        this->deleteEntity(entityManager, id, entity);
        ImGui::PopID();
        return true;
    }
    ImGui::Indent(20);
    for (auto &childId : entity.getChildren())
        if (this->drawEntityInSceneHierarchy(entityManager, childId, entityManager->getEntity(childId)))
            break;
    ImGui::Unindent(20);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
    ImGui::Dummy(ImVec2(ImGui::GetWindowWidth() / 2, 4));
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY")) {
            int draggedEntityId = *(const int *)payload->Data;
            auto draggedEntityIt = entityManager->getEntities().find(draggedEntityId);
            if (draggedEntityIt != entityManager->getEntities().end() && draggedEntityId != id) {
                if (draggedEntityIt->second.getParent().has_value()) {
                    entityManager->getEntity(draggedEntityIt->second.getParent().value()).removeChild(draggedEntityId);
                    draggedEntityIt->second.removeParent();
                    if (draggedEntityIt->second.hasComponent<Transform>() && entity.hasComponent<Transform>()) {
                        Transform &draggedTransform = draggedEntityIt->second.getComponent<Transform>();
                        Transform &entityTransform = entity.getComponent<Transform>();
                        draggedTransform.rotation = glm::inverse(entityTransform.rotation) * draggedTransform.rotation;
                        draggedTransform.position = glm::inverse(entityTransform.rotation) * (draggedTransform.position - entityTransform.position);
                    }
                }

                int direction = draggedEntityId < id ? 1 : -1;
                auto end = entityManager->getEntities().find(id);
                if (direction == -1)
                    end = std::next(end);
                for (auto swapIt = draggedEntityIt; swapIt != end; swapIt = std::next(swapIt, direction))
                    std::swap(swapIt->second, std::next(swapIt, direction)->second);
                this->_graphic->setSelectedEntity(end->first);
            }
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::PopID();
    return false;
}

void GUISystem::drawSceneHierarchy(std::shared_ptr<EntityManager> &entityManager)
{
    if (!ImGui::Begin(ICON_FA_LIST " Scene Hierarchy", &this->_showSceneHierarchy)) {
        ImGui::End();
        return;
    }
    if (ImGui::BeginCombo("Scenes", this->_resourceManager->getActiveSceneName().c_str())) {
        for (auto &[name, scene] : this->_resourceManager->getScenes()) {
            if (ImGui::Selectable(name.c_str(), this->_resourceManager->getActiveSceneName() == name)) {
                this->_resourceManager->setActiveScene(name);
                entityManager = scene->getEntityManager();
            }
        }
        ImGui::EndCombo();
    }
    if (this->_resourceManager->getActiveSceneName().empty()) {
        ImGui::End();
        return;
    }

    const float footerReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(ImGui::GetWindowWidth() - 8, -footerReserve));

    ImGui::Dummy(ImVec2(ImGui::GetWindowWidth() / 2, 4));
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY")) {
            int draggedEntityId = *(const int *)payload->Data;
            auto draggedEntityIt = entityManager->getEntities().find(draggedEntityId);
            if (draggedEntityIt != entityManager->getEntities().end()) {
                for (auto swapIt = draggedEntityIt; swapIt != entityManager->getEntities().begin(); --swapIt)
                    std::swap(swapIt->second, std::prev(swapIt)->second);
                this->_graphic->setSelectedEntity(entityManager->getEntities().begin()->first);
            }
        }
        ImGui::EndDragDropTarget();
    }
    for (auto &[id, entity] : entityManager->getEntities()) {
        if (entity.getParent().has_value())
            continue;
        if (this->drawEntityInSceneHierarchy(entityManager, id, entity))
            break;
    }

    ImGui::EndChild();

    ImGui::Separator();
    if (ImGui::Button(ICON_FA_PLUS " Add Entity")) {
        Entity entity;
        entityManager->addEntity(entity);
    }
    ImGui::End();
}

void GUISystem::drawProperties(std::shared_ptr<EntityManager> &entityManager)
{
    if (!ImGui::Begin(ICON_FA_INFO_CIRCLE " Properties", &this->_showProperties)) {
        ImGui::End();
        return;
    }

    if (!entityManager->hasEntity(this->_graphic->getSelectedEntity())) {
        ImGui::Text("No entity selected");
        ImGui::End();
        return;
    }
    const float footerReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerReserve));
    ImGui::Text("ID: %lu", this->_graphic->getSelectedEntity());
    Entity &entity = entityManager->getEntity(this->_graphic->getSelectedEntity());
    if (entity.hasComponent<Transform>()) {
        bool open = true;
        if (ImGui::CollapsingHeader(ICON_FA_INFO_CIRCLE " Transform", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            Transform &transform = entity.getComponent<Transform>();

            ImGui::DragFloat3("Position##Transform", (float *)&transform.position, 0.1f, -FLT_MAX, +FLT_MAX);
            glm::vec3 euler = glm::degrees(glm::eulerAngles(transform.rotation));
            ImGui::DragFloat3("Rotation##Transform", (float *)&euler, 1.0f, -FLT_MAX, +FLT_MAX);
            transform.rotation = glm::quat(glm::radians(euler));
            ImGui::DragFloat3("Scale##Transform", (float *)&transform.scale, 0.01f, 0.0f, +FLT_MAX);
            ImGui::Separator();
        }
        if (!open)
            entity.removeComponent<Transform>();
    }
    if (entity.hasComponent<Model>()) {
        bool open = true;
        if (ImGui::CollapsingHeader(ICON_FA_CUBE " Model", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
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
            ImGui::Text("Materials (%d)", this->_resourceManager->getModels()[selectedItem]->getMaterialNb());
            int i = 0;
            for (auto &[name, material] : model.materials) {
                ImGui::PushID(i);
                ImGui::DragFloat3((name + "##Material").c_str(), (float *)&material.diffuse, 0.01f, 0.0f, 1.0f);
                ImGui::PopID();
                i++;
            }
            if (this->_resourceManager->getModels()[selectedItem]->getMaterialNb() > model.materials.size()) {
                std::unordered_map<std::string, Material> &defaultMaterials = this->_resourceManager->getModels()[selectedItem]->getMaterials();

                if (ImGui::BeginCombo("Select Material##Model", "Select Material")) {
                    for (auto &[name, defaultMaterial] : defaultMaterials) {
                        if (model.materials.find(name) != model.materials.end())
                            continue;
                        if (ImGui::Selectable(name.c_str()))
                            model.materials[name] = defaultMaterial;
                    }
                    ImGui::EndCombo();
                }
            }
            ImGui::Text("Textures (%d)", this->_resourceManager->getModels()[selectedItem]->getTextureNb());
            ImGui::Separator();
        }
        if (!open)
            entity.removeComponent<Model>();
    }
    if (entity.hasComponent<Camera>()) {
        bool open = true;
        if (ImGui::CollapsingHeader(ICON_FA_VIDEO " Camera", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            Camera &camera = entity.getComponent<Camera>();

            ImGui::SliderInt("Type##Camera", (int *)&camera.type, 0, CameraType::CAMERA_TYPE_COUNT - 1, cameraTypeNames[camera.type]);
            ImGui::DragFloat3("Forward##Camera", (float *)&camera.forward, 0.01f, -1.0f, 1.0f);
            ImGui::SliderFloat("FOV##Camera", &camera.fov, 30.0f, 90.0f);
            ImGui::SliderFloat("Near##Camera", &camera.near, 0.1f, 100.0f);
            ImGui::SliderFloat("Far##Camera", &camera.far, camera.near + 0.1f, 1000.0f);
            ImGui::Separator();
        }
        if (!open)
            entity.removeComponent<Camera>();
    }
    if (entity.hasComponent<Light>()) {
        bool open = true;
        if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB " Light", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            Light &light = entity.getComponent<Light>();

            ImGui::SliderInt("Type##Light", (int *)&light.type, 0, LightType::LIGHT_TYPE_COUNT - 1, lightTypeNames[light.type]);
            if (light.type == LightType::DIRECTIONAL)
                ImGui::DragFloat3("ShadowSize##Light", (float *)&light.shadowSize, 0.1f, 0.0f, +FLT_MAX);
            ImGui::SliderFloat("Intensity##Light", &light.intensity, 0.0f, 16.0f);
            ImGui::ColorEdit3("Color##Light", (float *)&light.color);
            ImGui::SliderFloat("Cut Off##Light", &light.cutOff, 0.0f, 90.0f);
            ImGui::Separator();
        }
        if (!open)
            entity.removeComponent<Light>();
    }
    if (entity.hasComponent<Cubemap>()) {
        bool open = true;
        if (ImGui::CollapsingHeader(ICON_FA_MAP " Cubemap", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            Cubemap &cubemap = entity.getComponent<Cubemap>();

            std::string selectedItem = cubemap.name;
            if (ImGui::BeginCombo("Select Item##Cubemap", selectedItem.c_str())) {
                for (auto &[key, value] : this->_resourceManager->getSkyboxes()) {
                    bool isSelected = (selectedItem == key);
                    if (ImGui::Selectable(key.c_str(), isSelected))
                        cubemap.name = key;
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::Separator();
        }
        if (!open)
            entity.removeComponent<Cubemap>();
    }
    if (entity.hasComponent<Billboard>()) {
        bool open = true;
        if (ImGui::CollapsingHeader(ICON_FA_SIGN " Billboard", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            Billboard &billboard = entity.getComponent<Billboard>();

            this->drawTextureSelect("Texture##Billboard", billboard.name);
            ImGui::Checkbox("Lock Y Axis##Billboard", &billboard.lockYAxis);
            ImGui::Separator();
        }
        if (!open)
            entity.removeComponent<Billboard>();
    }
    if (entity.hasComponent<Map>()) {
        bool open = true;
        if (ImGui::CollapsingHeader(ICON_FA_MAP " Map", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            Map &map = entity.getComponent<Map>();

            this->drawTextureSelect("Height Map##Map", map.heightMap);
            this->drawTextureSelect("Diffuse Texture##Map", map.diffuseTexture);
            this->drawTextureSelect("Diffuse Texture 2##Map", map.diffuseTexture2);
            this->drawTextureSelect("Diffuse Texture 3##Map", map.diffuseTexture3);

            ImGui::Separator();
        }
        if (!open)
            entity.removeComponent<Map>();
    }
    if (entity.hasComponent<RigidBody>()) {
        bool open = true;
        if (ImGui::CollapsingHeader(ICON_FA_BOX " RigidBody", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            RigidBody &rigidBody = entity.getComponent<RigidBody>();

            bool isEdited = false;
            if (ImGui::SliderFloat("Mass##RigidBody", &rigidBody.mass, 0.0f, 100.0f))
                isEdited = true;
            if (ImGui::SliderFloat("Friction##RigidBody", &rigidBody.friction, 0.0f, 1.0f))
                isEdited = true;
            if (ImGui::SliderFloat("Restitution##RigidBody", &rigidBody.restitution, 0.0f, 1.0f))
                isEdited = true;
            if (ImGui::Checkbox("Kinematic##RigidBody", &rigidBody.kinematic))
                isEdited = true;
            if (isEdited && this->_graphic->isRunning()) {
                std::size_t instance = this->getPhysicInstanceIndex(this->_graphic->getSelectedEntity());
                if (instance != (std::size_t)-1) {
                    // this->_resourceManager->getPhysicInstances()[instance].updateRigidBodyRuntime(rigidBody);
                }
            }
            ImGui::Separator();
        }
        if (!open)
            entity.removeComponent<RigidBody>();
    }
    if (entity.hasComponent<Collider>()) {
        bool open = true;
        if (ImGui::CollapsingHeader(ICON_FA_BOXES " Collider", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            Collider &collider = entity.getComponent<Collider>();

            bool isEdited = false;
            if (ImGui::SliderInt("Type##Collider", (int *)&collider.type, 0, ColliderType::COLLIDER_TYPE_COUNT - 1, colliderTypeNames[collider.type]))
                isEdited = true;
            if (ImGui::DragFloat3("Center##Collider", (float *)&collider.center, 0.1f, -FLT_MAX, +FLT_MAX))
                isEdited = true;
            switch (collider.type) {
            case ColliderType::BOX:
                if (ImGui::DragFloat3("Size##Collider", (float *)&collider.size, 0.1f, -FLT_MAX, +FLT_MAX))
                    isEdited = true;
                break;
            case ColliderType::SPHERE:
                if (ImGui::DragFloat("Radius##Collider", &collider.size.x, 0.1f, -FLT_MAX, +FLT_MAX))
                    isEdited = true;
                break;
            case ColliderType::CAPSULE:
                if (ImGui::DragFloat2("Radius##Collider", (float *)&collider.size, 0.1f, -FLT_MAX, +FLT_MAX))
                    isEdited = true;
                break;
            default:
                break;
            }
            static char buffer[32];
            strcpy(buffer, collider.tag.c_str());
            if (ImGui::InputText("Tag##Collider", buffer, sizeof(buffer))) {
                collider.tag = buffer;
                isEdited = true;
            }
            if (isEdited && this->_graphic->isRunning()) {
                std::size_t instance = this->getPhysicInstanceIndex(this->_graphic->getSelectedEntity());
                // this->_resourceManager->getPhysicInstances()[instance].updateColliderRuntime(collider);
            }
            ImGui::Separator();
        }
        if (!open)
            entity.removeComponent<Collider>();
    }
    if (entity.hasComponent<CharacterController>()) {
        bool open = true;
        if (ImGui::CollapsingHeader(ICON_FA_RUNNING " CharacterController", &open, ImGuiTreeNodeFlags_DefaultOpen)) {
            CharacterController &characterController = entity.getComponent<CharacterController>();

            ImGui::DragFloat3("Center##CharacterController", (float *)&characterController.center, 1.0f, -FLT_MAX, +FLT_MAX);
            ImGui::SliderFloat("SlopeLimit##CharacterController", &characterController.slopeLimit, 0.0f, 90.0f);
            ImGui::Separator();
        }
        if (!open)
            entity.removeComponent<CharacterController>();
    }
    std::size_t it = 0;
    for (auto &[scriptName, script] : this->_resourceManager->getScripts()) {
        if (entity.hasScriptComponent(scriptName)) {
            bool open = true;
            if (ImGui::CollapsingHeader((ICON_FA_FILE_CODE " " + scriptName).c_str(), &open, ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::PushID(it);
                for (auto &[fieldName, field] : script.getFields()) {
                    if (this->_graphic->isRunning()) {
                        std::size_t instanceIndex = entity.getScriptIndexes()[scriptName];
                        if (field.type == "Single") {
                            float value = this->_resourceManager->getScriptInstances()[instanceIndex].getFieldValue<float>(fieldName);
                            if (ImGui::DragFloat(fieldName.c_str(), &value))
                                this->_resourceManager->getScriptInstances()[instanceIndex].setFieldValue(fieldName, value);
                        } else if (field.type == "Entity" || field.type == "UInt64") {
                            unsigned long value = this->_resourceManager->getScriptInstances()[instanceIndex].getFieldValue<unsigned long>(fieldName);
                            const ImU64 increment = 1;
                            if (ImGui::InputScalar(fieldName.c_str(), ImGuiDataType_U64, &value, &increment))
                                this->_resourceManager->getScriptInstances()[instanceIndex].setFieldValue(fieldName, value);
                        } else if (field.type == "String") {
                            // std::string value = this->_resourceManager->getScriptInstances()[instanceIndex].getFieldValue<std::string>(fieldName);
                            std::string value = entity.getScriptComponent(scriptName).getField<std::string>(fieldName);
                            static char buffer[256];
                            strcpy(buffer, value.c_str());
                            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
                            if (ImGui::InputText(fieldName.c_str(), buffer, sizeof(buffer), ImGuiInputTextFlags_ReadOnly)) {
                                // TODO fix access field value
                                // this->_resourceManager->getScriptInstances()[instanceIndex].setFieldValue(fieldName, std::string(buffer));
                            }
                            ImGui::PopStyleColor();
                        }
                    } else {
                        if (field.type == "Single") {
                            float &value = entity.getScriptComponent(scriptName).getField<float>(fieldName);
                            ImGui::DragFloat(fieldName.c_str(), &value);
                        } else if (field.type == "Entity" || field.type == "UInt64") {
                            unsigned long &value = entity.getScriptComponent(scriptName).getField<unsigned long>(fieldName);
                            const ImU64 increment = 1;
                            ImGui::InputScalar(fieldName.c_str(), ImGuiDataType_U64, &value, &increment);
                        } else if (field.type == "String") {
                            std::string &value = entity.getScriptComponent(scriptName).getField<std::string>(fieldName);
                            static char buffer[256];
                            strcpy(buffer, value.c_str());
                            if (ImGui::InputText(fieldName.c_str(), buffer, sizeof(buffer)))
                                value = buffer;
                        }
                    }
                }
                ImGui::PopID();
                ImGui::Separator();
            }
            if (!open)
                entity.removeScriptComponent(scriptName);
            it++;
        }
    }

    ImGui::EndChild();
    ImGui::Separator();
    ImGui::SetNextItemWidth(-1);
    if (ImGui::BeginCombo("##Add", ICON_FA_PLUS " Add Component", ImGuiComboFlags_NoArrowButton)) {
        if (!entity.hasComponent<Transform>()) {
            ImGui::Text(ICON_FA_ARROWS_ALT);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable("Transform##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Transform());
        }
        if (!entity.hasComponent<Model>()) {
            ImGui::Text(ICON_FA_CUBE);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable("Model##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Model());
        }
        if (!entity.hasComponent<Camera>()) {
            ImGui::Text(ICON_FA_CAMERA);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable("Camera##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Camera());
        }
        if (!entity.hasComponent<Light>()) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 2);
            ImGui::Text(ICON_FA_LIGHTBULB);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable("Light##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Light());
        }
        if (!entity.hasComponent<Cubemap>()) {
            ImGui::Text(ICON_FA_CUBE);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable("Cubemap##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Cubemap());
        }
        if (!entity.hasComponent<Billboard>()) {
            ImGui::Text(ICON_FA_SIGN);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable("Billboard##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Billboard());
        }
        if (!entity.hasComponent<Map>()) {
            ImGui::Text(ICON_FA_MAP);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable("Map##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Map());
        }
        if (!entity.hasComponent<RigidBody>()) {
            ImGui::Text(ICON_FA_BOXES);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable("RigidBody##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(RigidBody());
        }
        if (!entity.hasComponent<Collider>()) {
            ImGui::Text(ICON_FA_BOXES);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable("Collider##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(Collider());
        }
        if (!entity.hasComponent<CharacterController>()) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 2);
            ImGui::Text(ICON_FA_RUNNING);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable("CharacterController##selectable", false, ImGuiSelectableFlags_SpanAllColumns))
                entity.addComponent(CharacterController());
        }

        it = 0;
        for (auto &[scriptName, script] : this->_resourceManager->getScripts()) {
            if (entity.hasScriptComponent(scriptName))
                continue;
            ImGui::PushID((int)it);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 2);
            ImGui::Text(ICON_FA_FILE_CODE);
            ImGui::SameLine(20, 0);
            if (ImGui::Selectable((scriptName + "##selectable").c_str(), false, ImGuiSelectableFlags_SpanAllColumns)) {
                ScriptComponent scriptComponent(script);
                entity.addScriptComponent(scriptName, scriptComponent);
            }
            ImGui::PopID();
            it++;
        }
        ImGui::EndCombo();
    }

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
        for (auto &[type, line] : this->_consoleBuffer) {
            switch (type) {
            case ToastType::ERROR:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.905f, 0.298f, 0.235f, 1.0f));
                break;
            case ToastType::WARNING:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.945f, 0.768f, 0.058f, 1.0f));
                break;
            case ToastType::INFO:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.203f, 0.596f, 0.858f, 1.0f));
                break;
            case ToastType::SUCCESS:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.027f, 0.737f, 0.047f, 1.0f));
                break;
            default:
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                break;
            }
            ImGui::TextWrapped("%s", line.c_str());
            ImGui::PopStyleColor();
        }
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

    GLuint texture = this->_graphic->getFrameBuffers()[this->_graphic->isPostProcessing() ? "final" : "render"].texture;
    ImGui::Image((void *)(intptr_t)texture, ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

void GUISystem::drawScene(std::shared_ptr<EntityManager> &entityManager)
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
    if (this->_showTools) {
        ImGui::SetCursorScreenPos(ImVec2(this->_graphic->getSceneViewPort().x + 10, this->_graphic->getSceneViewPort().y + 10));
        if (ImGui::RadioButton("Translate", this->_currentOperation == ImGuizmo::TRANSLATE))
            this->_currentOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", this->_currentOperation == ImGuizmo::ROTATE))
            this->_currentOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", this->_currentOperation == ImGuizmo::SCALE))
            this->_currentOperation = ImGuizmo::SCALE;
        if (this->_currentOperation != ImGuizmo::SCALE) {
            ImGui::SameLine();
            ImGui::Text("|");
            ImGui::SameLine();
            if (ImGui::RadioButton("Local", this->_currentMode == ImGuizmo::LOCAL))
                this->_currentMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("Global", this->_currentMode == ImGuizmo::WORLD))
                this->_currentMode = ImGuizmo::WORLD;
        } else
            this->_currentMode = ImGuizmo::LOCAL;
        ImGuizmo::BeginFrame();
        ImGuizmo::SetDrawlist();
        if (entityManager->hasEntity(this->_graphic->getSelectedEntity()))
            guizmoDrawn = this->drawGuizmo(entityManager->getEntity(this->_graphic->getSelectedEntity()), entityManager);

        if (!ImGui::GetIO().KeyCtrl && !ImGuizmo::IsUsing()) {
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_T)))
                this->_currentOperation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_R)))
                this->_currentOperation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)))
                this->_currentOperation = ImGuizmo::SCALE;
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_L)))
                this->_currentMode = ImGuizmo::LOCAL;
            if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_G)))
                this->_currentMode = ImGuizmo::WORLD;
        }

        glm::vec4 viewport = this->_graphic->getSceneViewPort();
        ImVec2 cubePos(viewport.x + viewport.z - 128, viewport.y);
        glm::mat4 view = this->_graphic->getRenderView().getView();
        ImGuizmo::ViewManipulate((float *)&view, 0.0f, cubePos, ImVec2(128, 128), 0x10101010);
        // TODO move camera
    }
    this->_graphic->setSceneHovered(ImGui::IsWindowHovered() && !(ImGuizmo::IsOver() && guizmoDrawn));

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("FILE")) {
            std::string file = (char *)payload->Data;
            if (file.substr(file.find_last_of('.') + 1) == "dae") {
                Entity entity;
                entity.addComponent(Transform());
                entity.addComponent(Model());
                entity.getComponent<Model>().name = file.substr(0, file.find_last_of('.'));
                entityManager->addEntity(entity);
            }
            std::cout << "[Toast Info]Dropped file: " << file << std::endl;
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::End();
}

bool GUISystem::drawGuizmo(Entity &entity, std::shared_ptr<EntityManager> &entityManager)
{
    if (!entity.hasComponent<Transform>())
        return false;
    Transform &transform = entity.getComponent<Transform>();
    std::optional<Transform> parentTransform = entity.getParent().has_value() && entityManager->getEntity(entity.getParent().value()).hasComponent<Transform>()
                                                   ? std::optional<Transform>(entityManager->getEntity(entity.getParent().value()).getComponent<Transform>())
                                                   : std::nullopt;

    glm::vec4 viewport = this->_graphic->getSceneViewPort();
    glm::mat4 view = this->_graphic->getRenderView().getView();
    glm::mat4 projection = this->_graphic->getRenderView().getProjection();
    glm::mat4 model = glm::mat4(1.0f);

    float snapValue = this->_currentOperation == ImGuizmo::ROTATE ? 45.0f : 0.5f;
    float snap[3] = {snapValue, snapValue, snapValue};

    if (parentTransform.has_value()) {
        transform.rotation = parentTransform.value().rotation * transform.rotation;
        transform.position = parentTransform.value().rotation * transform.position + parentTransform.value().position;
    }
    glm::vec3 euler = glm::degrees(glm::eulerAngles(transform.rotation));
    ImGuizmo::SetRect(viewport.x, viewport.y, viewport.z, viewport.w);
    ImGuizmo::RecomposeMatrixFromComponents((float *)&transform.position, (float *)&euler, (float *)&transform.scale, (float *)&model);
    ImGuizmo::Manipulate((float *)&view, (float *)&projection, this->_currentOperation, this->_currentMode, (float *)&model, nullptr, ImGui::GetIO().KeyCtrl ? snap : nullptr);
    ImGuizmo::DecomposeMatrixToComponents((float *)&model, (float *)&transform.position, (float *)&euler, (float *)&transform.scale);
    transform.rotation = glm::quat(glm::radians(euler));
    if (parentTransform.has_value()) {
        transform.rotation = glm::inverse(parentTransform.value().rotation) * transform.rotation;
        transform.position = glm::inverse(parentTransform.value().rotation) * (transform.position - parentTransform.value().position);
    }
    return ImGuizmo::IsUsing();
}

void GUISystem::drawFileExplorer()
{
    if (!ImGui::Begin(ICON_FA_FOLDER_OPEN " File Explorer", &this->_showFileExplorer)) {
        ImGui::End();
        return;
    }
    if (this->_currentProject.empty()) {
        ImVec2 textSize = ImGui::CalcTextSize("No scene loaded");
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) / 2);
        ImGui::SetCursorPosY((ImGui::GetWindowHeight() - textSize.y) / 2);
        ImGui::Text("No project loaded");
        ImGui::End();
        return;
    }

    int columns = (int)ImGui::GetContentRegionAvail().x / (50 + 20);
    if (columns < 1)
        columns = 1;

    if (this->_projectSettings[this->_currentProject].rootPath == this->_fileExplorerPath) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleColor(ImGuiCol_Button, BUTTON_COLOR_DISABLED);
        ImGui::Button(ICON_FA_ARROW_LEFT " ..");
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();
    } else {
        if (ImGui::Button(ICON_FA_ARROW_LEFT " .."))
            this->_fileExplorerPath = std::filesystem::path(this->_fileExplorerPath).parent_path().string();
    }
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
                // TODO prefabs and files opening
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

void GUISystem::drawAbout()
{
    if (!ImGui::Begin(ICON_FA_INFO_CIRCLE " About", &this->_showAbout, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }
    ImGui::Text("LushEngine");
    ImGui::End();
}

bool GUISystem::openFolderBrowser(const std::string &title, std::string &path, bool &modal)
{
    if (!ImGui::Begin(title.c_str(), &this->_showRootBrowser, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking)) {
        ImGui::End();
        return false;
    }
    if (ImGui::Button(ICON_FA_ARROW_LEFT " .."))
        this->_fileBrowserPath = std::filesystem::path(this->_fileBrowserPath).parent_path().string();
    ImGui::SameLine();
    ImGui::Text("%s", this->_fileBrowserPath.c_str());
    for (const auto &entry : std::filesystem::directory_iterator(this->_fileBrowserPath)) {
        const auto &subPath = entry.path();
        const auto &filenameStr = subPath.filename().string();
        if (entry.is_directory() && filenameStr[0] != '.') {
            if (ImGui::Selectable(filenameStr.c_str(), false, ImGuiSelectableFlags_DontClosePopups)) {
                this->_fileBrowserPath = subPath.string();
            }
        }
    }
    const float footerReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerReserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::EndChild();
    ImGui::Separator();
    if (ImGui::Button("Cancel"))
        modal = false;
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Open").x - ImGui::GetStyle().ItemSpacing.x * 2, 0));
    ImGui::SameLine();
    if (ImGui::Button("Open")) {
        path = this->_fileBrowserPath;
        modal = false;
        ImGui::End();
        return true;
    }
    ImGui::End();
    return false;
}

void GUISystem::drawProjectManager(std::shared_ptr<EntityManager> &entityManager)
{
    ImGui::SetNextWindowSize(ImVec2(600, 400));
    ImGuiWindowClass windowClass;
    windowClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoDockingOverMe | ImGuiDockNodeFlags_NoDockingOverOther | ImGuiDockNodeFlags_NoDockingSplitOther;
    ImGui::SetNextWindowClass(&windowClass);
    if (!ImGui::Begin("Project Manager", &this->_showProjectManager, ImGuiWindowFlags_NoResize)) {
        ImGui::End();
        return;
    }
    if (!this->_editingProject.empty()) {
        if (ImGui::Button(ICON_FA_ANGLE_LEFT, ImVec2(20, 40))) {
            this->_editingProject.clear();
            ImGui::End();
            return;
        }
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        bool isEdited = false;

        static char buffer[32];
        strcpy(buffer, this->_editingProject.c_str());
        if (ImGui::InputText("##ProjectName", buffer, sizeof(buffer))) {
            this->_projectSettings[buffer] = this->_projectSettings[this->_editingProject];
            this->_projectSettings.erase(this->_editingProject);
            this->_editingProject = buffer;
            isEdited = true;
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 50);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        if (ImGui::ColorEdit3("##Project Color", (float *)&this->_projectSettings[this->_editingProject].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
            isEdited = true;

        ImGui::Separator();

        ImGui::Text("Root Path: %s", this->_projectSettings[this->_editingProject].rootPath.c_str());
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FOLDER_OPEN " Browse...##Root", ImVec2(20, 20)) && !this->_showBuildBrowser) {
            this->_showRootBrowser = true;
        }
        ImGui::Text("Build Path: %s", this->_projectSettings[this->_editingProject].buildPath.c_str());
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FOLDER_OPEN " Browse...##Build", ImVec2(20, 20)) && !this->_showRootBrowser) {
            this->_showBuildBrowser = true;
        }

        ImGui::Separator();
        ImGui::Text("Executable name:");
        ImGui::SameLine();
        static char buffer2[32];
        strcpy(buffer2, this->_projectSettings[this->_editingProject].execName.c_str());
        if (ImGui::InputText("##ProjectExecName", buffer2, sizeof(buffer2))) {
            this->_projectSettings[this->_editingProject].execName = buffer2;
            isEdited = true;
        }
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 15);
        ImGui::Text("Icon:");
        ImGui::SameLine();
        if (this->drawTextureSelect("##Icon", this->_projectSettings[this->_editingProject].iconName))
            isEdited = true;
        if (this->_projectSettings[this->_editingProject].iconName != "None") {
            ImGui::SameLine();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 15);
            if (this->_resourceManager->getTextures().contains(this->_projectSettings[this->_editingProject].iconName)) {
                GLuint texture = this->_resourceManager->getTextures()[this->_projectSettings[this->_editingProject].iconName]->getId();
                ImGui::Image((void *)(intptr_t)texture, ImVec2(50, 50));
            }
        }
        ImGui::Separator();
        ImGui::Text("Scenes in Build:");
        if (this->_currentProject == this->_editingProject) {
            for (auto &[sceneName, scene] : this->_resourceManager->getScenes()) {
                bool isUsed = std::find(this->_projectSettings[this->_editingProject].scenes.begin(), this->_projectSettings[this->_editingProject].scenes.end(), sceneName) !=
                              this->_projectSettings[this->_editingProject].scenes.end();
                if (ImGui::Checkbox(sceneName.c_str(), &isUsed)) {
                    scene->setUsed(isUsed);
                    if (isUsed)
                        this->_projectSettings[this->_editingProject].scenes.push_back(sceneName);
                    else
                        this->_projectSettings[this->_editingProject].scenes.erase(
                            std::find(this->_projectSettings[this->_editingProject].scenes.begin(), this->_projectSettings[this->_editingProject].scenes.end(), sceneName));
                    isEdited = true;
                }
                if (sceneName == "main") {
                    ImGui::SameLine();
                    ImGui::Text("(Start scene)");
                }
            }
        } else {
            ImGui::SameLine();
            ImGui::Text(ICON_FA_EXCLAMATION_TRIANGLE " Open project to edit scenes");
            for (auto &sceneName : this->_projectSettings[this->_editingProject].scenes) {
                ImGui::Text("   %s", sceneName.c_str());
                if (sceneName == "main") {
                    ImGui::SameLine();
                    ImGui::Text("(Start scene)");
                }
            }
        }

        if (this->_showRootBrowser) {
            if (this->openFolderBrowser("Choose root directory", this->_projectSettings[this->_editingProject].rootPath, this->_showRootBrowser)) {
                isEdited = true;
            }
        } else if (this->_showBuildBrowser) {
            if (this->openFolderBrowser("Choose build directory", this->_projectSettings[this->_editingProject].buildPath, this->_showBuildBrowser)) {
                isEdited = true;
            }
        }

        if (isEdited)
            this->saveProjectSettings();
        ImGui::End();
        return;
    }

    bool disableAll = !this->_currentProject.empty();
    for (auto it = this->_projectSettings.begin(); it != this->_projectSettings.end();) {
        std::string projectName = it->first;
        ProjectSettings &project = it->second;

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(project.color.x, project.color.y, project.color.z, 1.0f));
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::Button(("##ProjectColor" + projectName).c_str(), ImVec2(10, 40));
        ImGui::PopItemFlag();
        ImGui::SameLine();
        ImGui::SetCursorPosX(22);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 1);
        if (disableAll) {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleColor(ImGuiCol_Button, BUTTON_COLOR_DISABLED);
        }
        if (ImGui::Selectable(("##" + projectName).c_str(), this->_currentProject == projectName, 0, ImVec2(ImGui::GetWindowWidth() - 100, 36))) {
            glfwSetWindowTitle(this->_graphic->getWindow(), std::string("Lush Engine - " + projectName).c_str());
            this->_currentProject = projectName;
            this->_resourceManager->loadProject(project.rootPath);
            entityManager = this->_resourceManager->getActiveScene().getEntityManager();
            for (auto &sceneName : project.scenes) {
                if (this->_resourceManager->getScenes().contains(sceneName))
                    this->_resourceManager->getScenes()[sceneName]->setUsed(true);
            }
            this->_resourceManager->setLogo(project.iconName);
            this->_fileExplorerPath = project.rootPath;
            this->_showProjectManager = false;
        }
        if (disableAll) {
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::SetCursorPosX(25);
        ImGui::Text("%s", projectName.c_str());
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 70);
        ImGui::PushStyleColor(ImGuiCol_Button, BUTTON_COLOR_RED);
        if (ImGui::Button((ICON_FA_TRASH "##" + projectName).c_str(), ImVec2(40, 40))) {
            it = this->_projectSettings.erase(it);
            this->saveProjectSettings();
            ImGui::PopStyleColor();
            continue;
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 30);
        if (ImGui::Button((ICON_FA_ANGLE_RIGHT "##" + projectName).c_str(), ImVec2(30, 40))) {
            this->_editingProject = projectName;
        }
        ++it;
    }
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 20);
    if (ImGui::Button(ICON_FA_PLUS, ImVec2(40, 40))) {
        std::string currentPath = std::filesystem::current_path().string();
        this->_projectSettings["New Project"] = ProjectSettings(currentPath, currentPath, "New Project");
        this->saveProjectSettings();
        this->_editingProject = "New Project";
    }
    ImGui::End();
}

std::size_t GUISystem::getPhysicInstanceIndex(std::size_t entityId)
{
    std::size_t i = 0;
    for (auto &instance : this->_resourceManager->getPhysicInstances()) {
        if (instance->getId() == entityId)
            return i;
        i++;
    }
    return (std::size_t)-1;
}

bool GUISystem::drawTextureSelect(const std::string &fieldName, std::string &texture)
{
    bool isEdited = false;
    std::string selectedItem = texture;
    if (ImGui::BeginCombo(fieldName.c_str(), selectedItem.c_str())) {
        for (auto &[key, value] : this->_resourceManager->getTextures()) {
            bool isSelected = (selectedItem == key);
            if (ImGui::Selectable(key.c_str(), isSelected)) {
                texture = key;
                isEdited = true;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        if (ImGui::Selectable("None", selectedItem == "None")) {
            texture = "None";
            isEdited = true;
        }
        ImGui::EndCombo();
    }
    return isEdited;
}

void GUISystem::build()
{
    if (this->_currentProject.empty()) {
        std::cout << "[Toast Warning]No project opened" << std::endl;
        return;
    }

    ProjectSettings project = this->_projectSettings[this->_currentProject];
    std::filesystem::create_directories(project.buildPath + "/Data/mono/4.5");

    this->_resourceManager->serializeAssetPack(project.buildPath + "/Data/AssetPack.data");
    std::cout << "[Toast Success]AssetPack serialized at location: " << project.buildPath << "/Data/AssetPack.data" << std::endl;
    std::filesystem::copy_file("lushGame", std::filesystem::path(project.buildPath) / (std::filesystem::path(project.execName).filename().string()),
                               std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy("Resources/bin", std::filesystem::path(project.buildPath) / "Data",
                          std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
    std::filesystem::copy_file("libs/mono/4.5/mscorlib.dll", std::filesystem::path(project.buildPath) / "Data/mono/4.5/mscorlib.dll",
                               std::filesystem::copy_options::overwrite_existing);
    std::cout << "[Toast Success]Build complete!" << std::endl;
}
