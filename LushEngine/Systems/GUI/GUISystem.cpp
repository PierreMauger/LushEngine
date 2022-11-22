#include "Systems/GUI/GUISystem.hpp"

using namespace Lush;

GUISystem::GUISystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;

    if (!IMGUI_CHECKVERSION())
        throw std::runtime_error("IMGUI version is invalid");
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(this->_graphic->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 410");
    ImGuizmo::AllowAxisFlip(false); // doesn't work sadly

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableKeyboard;
    io.Fonts->AddFontDefault();

    // merge in icons from Font Awesome
    static const ImWchar iconsRanges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig iconsConfig;
    iconsConfig.MergeMode = true;
    iconsConfig.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("Resources/Fonts/" FONT_ICON_FILE_NAME_FAS, 12.0f, &iconsConfig, iconsRanges);
}

GUISystem::~GUISystem()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUISystem::update(EntityManager &entityManager, ComponentManager &componentManager)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    this->setDock();
    this->drawMenuBar();
    this->drawActionBar();

    if (this->_showSceneHierarchy)
        this->drawSceneHierarchy(entityManager, componentManager);
    if (this->_showProperties)
        this->drawProperties(entityManager, componentManager);
    if (this->_showTools)
        this->drawTools();
    if (this->_showConsole)
        this->drawConsole();
    if (this->_showScene)
        this->drawScene(entityManager, componentManager);
    if (this->_showGame)
        this->drawGame();
    if (this->_showFileExplorer)
        this->drawFiles();
    if (this->_showProfiler)
        this->drawProfiler();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUISystem::setDock()
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos({viewport->Pos.x, viewport->Pos.y + 20}); // 20 is the height of the action bar
    ImGui::SetNextWindowSize({viewport->Size.x, viewport->Size.y - 20});
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
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Scene Hierarchy", NULL, &this->_showSceneHierarchy);
            ImGui::MenuItem("Properties", NULL, &this->_showProperties);
            ImGui::MenuItem("Tools", NULL, &this->_showTools);
            ImGui::MenuItem("Console", NULL, &this->_showConsole);
            ImGui::MenuItem("Scene", NULL, &this->_showScene);
            ImGui::MenuItem("Game", NULL, &this->_showGame);
            ImGui::MenuItem("File Explorer", NULL, &this->_showFileExplorer);
            ImGui::MenuItem("Profiler", NULL, &this->_showProfiler);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void GUISystem::drawActionBar()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;

    if (ImGui::BeginViewportSideBar("##ActionBar", ImGui::GetMainViewport(), ImGuiDir_Up, ImGui::GetFrameHeight(), window_flags)) {
        if (ImGui::BeginMenuBar()) {
            ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x / 2 - 50, 0));
            ImGui::Button(ICON_FA_PLAY, ImVec2(45, 0));
            ImGui::SameLine(0, 10);
            ImGui::Button(ICON_FA_STOP, ImVec2(45, 0));
            ImGui::EndMenuBar();
        }
        ImGui::End();
    }
}

void GUISystem::drawSceneHierarchy(EntityManager &entityManager, ComponentManager &componentManager)
{
    std::size_t size = componentManager.getComponentArray().size();

    ImGui::Begin("Scene Hierarchy", &this->_showSceneHierarchy);
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
            masks[i].has_value() ? ImGui::Text("%s", this->formatBool(masks[i].value(), size).c_str()) : ImGui::Text("None");
            ImGui::TableNextColumn();
            if (masks[i].has_value()) {
                if (ImGui::Button(std::string("Remove##" + std::to_string(i)).c_str())) {
                    entityManager.removeMask(i);
                    componentManager.removeAllComponents(i);
                }
                ImGui::SameLine();
                if (ImGui::Button(std::string("Modify##" + std::to_string(i)).c_str()))
                    this->_selectedEntity = i;
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

void GUISystem::drawProperties(EntityManager &entityManager, ComponentManager &componentManager)
{
    ImGui::Begin("Properties", &this->_showProperties);
    auto &masks = entityManager.getMasks();

    ImGui::Text("ID: %lu", this->_selectedEntity);
    if (!masks[this->_selectedEntity].has_value()) {
        ImGui::Text("Entity does not exist");
        ImGui::End();
        return;
    }
    for (std::size_t i = 0; i < componentManager.getComponentArray().size(); i++) {
        if (masks[this->_selectedEntity].value() & (1 << i)) {
            if (ImGui::CollapsingHeader(FORMAT_NAME(componentManager.getComponentType(i).name()), ImGuiTreeNodeFlags_DefaultOpen)) {
                switch (i) {
                case 0: {
                    Transform &transform = componentManager.getComponent<Transform>(this->_selectedEntity);
                    ImGui::DragFloat3("Position##Transform", (float *)&transform.position, 0.1f, -FLT_MAX, +FLT_MAX);
                    ImGui::DragFloat3("Rotation##Transform", (float *)&transform.rotation, 1.0f, -FLT_MAX, +FLT_MAX);
                    ImGui::SliderFloat3("Scale##Transform", (float *)&transform.scale, 0.0f, 5.0f);
                    break;
                }
                case 1: {
                    Velocity &velocity = componentManager.getComponent<Velocity>(this->_selectedEntity);
                    ImGui::SliderFloat("X##Velocity", &velocity.x, -10, 10);
                    ImGui::SliderFloat("Y##Velocity", &velocity.y, -10, 10);
                    ImGui::SliderFloat("Z##Velocity", &velocity.z, -10, 10);
                    break;
                }
                case 2: {
                    Model &model = componentManager.getComponent<Model>(this->_selectedEntity);
                    const ImU64 increment = 1;
                    ImGui::InputScalar("ID##Model", ImGuiDataType_U64, &model.id, &increment);
                    break;
                }
                case 3: {
                    Camera &camera = componentManager.getComponent<Camera>(this->_selectedEntity);
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
                        ImGui::Checkbox("Align Target##Camera", &camera.alignTarget);
                    }
                    break;
                }
                case 4: {
                    Light &light = componentManager.getComponent<Light>(this->_selectedEntity);
                    const char *names[LightType::LIGHT_TYPE_COUNT] = {"Dir", "Point", "Spot", "Area"};

                    ImGui::SliderInt("Type##Light", (int *)&light.type, 0, LightType::LIGHT_TYPE_COUNT - 1, names[light.type]);
                    ImGui::SliderFloat("Intensity##Light", &light.intensity, 0.0f, 1.0f);
                    ImGui::ColorEdit3("Color##Light", (float *)&light.color);
                    ImGui::SliderFloat("Cut Off##Light", &light.cutOff, 0.0f, 90.0f);
                    break;
                }
                case 5: {
                    Control &control = componentManager.getComponent<Control>(this->_selectedEntity);
                    ImGui::Checkbox("##Control", &control.control);
                    break;
                }
                case 6: {
                    CubeMap &cubemap = componentManager.getComponent<CubeMap>(this->_selectedEntity);
                    const ImU64 increment = 1;
                    ImGui::InputScalar("ID##CubeMap", ImGuiDataType_U64, &cubemap.id, &increment);
                    ImGui::DragFloat("Rotation Speed##CubeMap", &cubemap.rotationSpeed, 0.1f, -FLT_MAX, +FLT_MAX);
                    ImGui::ColorEdit3("Color##CubeMap", (float *)&cubemap.color);
                    break;
                }
                case 7: {
                    BillBoard &bill = componentManager.getComponent<BillBoard>(this->_selectedEntity);
                    const ImU64 increment = 1;
                    ImGui::InputScalar("Texture ID##BillBoard", ImGuiDataType_U64, &bill.textureId, &increment);
                    break;
                }
                default:
                    break;
                }
                // TODO: Clear this switch too
                if (ImGui::Button(std::string("Remove##" + std::to_string(i)).c_str())) {
                    entityManager.updateMask(this->_selectedEntity, masks[this->_selectedEntity].value() & ~(1 << i));
                    componentManager.removeSingleComponent(this->_selectedEntity, componentManager.getComponentType(i));
                }
            }
            ImGui::Separator();
        }
    }
    const float footerReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerReserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::EndChild();
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Add Component")) {
        for (std::size_t i = 0; i < componentManager.getComponentArray().size(); i++) {
            if (!(masks[this->_selectedEntity].value() & (1 << i))) {
                if (ImGui::Selectable(FORMAT_NAME(componentManager.getComponentType(i).name()))) {
                    entityManager.updateMask(this->_selectedEntity, masks[this->_selectedEntity].value() | (1 << i));
                    switch (i) {
                    case 0:
                        componentManager.addComponent<Transform>(this->_selectedEntity);
                        break;
                    case 1:
                        componentManager.addComponent<Velocity>(this->_selectedEntity);
                        break;
                    case 2:
                        componentManager.addComponent<Model>(this->_selectedEntity);
                        break;
                    case 3:
                        componentManager.addComponent<Camera>(this->_selectedEntity);
                        break;
                    case 4:
                        componentManager.addComponent<Light>(this->_selectedEntity);
                        break;
                    case 5:
                        componentManager.addComponent<Control>(this->_selectedEntity);
                        break;
                    case 6:
                        componentManager.addComponent<CubeMap>(this->_selectedEntity);
                        break;
                    case 7:
                        componentManager.addComponent<BillBoard>(this->_selectedEntity);
                        break;
                    default:
                        break;
                    }
                    // TODO: replace this switch
                }
            }
        }
    }
    ImGui::End();
}

void GUISystem::drawTools()
{
    ImGui::Begin("Tools", &this->_showTools);
    if (ImGui::RadioButton("Translate", this->_currentOperation == ImGuizmo::TRANSLATE))
        this->_currentOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", this->_currentOperation == ImGuizmo::ROTATE))
        this->_currentOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", this->_currentOperation == ImGuizmo::SCALE))
        this->_currentOperation = ImGuizmo::SCALE;
    ImGui::SameLine();
    ImGui::Text("|");
    ImGui::SameLine();
    if (this->_currentOperation != ImGuizmo::SCALE) {
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
    ImGui::Begin("Console", &this->_showConsole);
    ImGui::End();
}

void GUISystem::drawScene(EntityManager &entityManager, ComponentManager &componentManager)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleVar(3);

    const float headerSize = ImGui::GetStyle().WindowPadding.y * 2.0f;
    this->_graphic->setViewPort({ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + headerSize, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y});

    GLuint texture = this->_graphic->getFrameBuffers()[0].texture;
    ImGui::Image((void *)(intptr_t)texture, ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y), ImVec2(0, 1), ImVec2(1, 0));
    this->drawGuizmo(entityManager, componentManager);
    ImGui::End();
}

void GUISystem::drawGuizmo(EntityManager &entityManager, ComponentManager &componentManager)
{
    ImGuizmo::BeginFrame();
    glm::mat4 view = this->_graphic->getRenderView().getView();
    glm::mat4 projection = this->_graphic->getRenderView().getProjection();

    if (!entityManager.hasMask(this->_selectedEntity, ComponentType::TRANSFORM))
        return;
    Transform &transform = componentManager.getComponent<Transform>(this->_selectedEntity);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model *= glm::toMat4(glm::quat(glm::radians(transform.rotation)));
    model = glm::scale(model, transform.scale);

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
    ImGuizmo::Manipulate(&view[0][0], &projection[0][0], this->_currentOperation, this->_currentMode, &model[0][0], nullptr, nullptr);

    ImGuizmo::DecomposeMatrixToComponents(&model[0][0], &transform.position[0], &transform.rotation[0], &transform.scale[0]);
}

void GUISystem::drawGame()
{
    ImGui::Begin("Game", &this->_showGame);
    ImGui::End();
}

void GUISystem::drawFiles()
{
    ImGui::Begin("File Explorer", &this->_showFileExplorer);
    ImGui::End();
}

void GUISystem::drawProfiler()
{
    ImGui::Begin("Profiler", &this->_showProfiler);
    ImGui::End();
}

std::string GUISystem::formatBool(std::size_t value, std::size_t size)
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
