#include "Systems/GUI/GUISystem.hpp"

#include "IconsFontAwesome5.h"

using namespace Lush;

GUISystem::GUISystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;

    if (!IMGUI_CHECKVERSION())
        throw std::runtime_error("IMGUI version is invalid");
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(this->_graphic->getWindow().get(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
    ImGuizmo::AllowAxisFlip(false); // doesn't work sadly

    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

    // merge in icons from Font Awesome
    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF("Resources/Fonts/" FONT_ICON_FILE_NAME_FAS, 12.0f, &icons_config, icons_ranges);
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

    this->drawMenuBar();
    this->drawActionBar();

    if (this->_showEntityManager)
        this->drawEntityManager(entityManager, componentManager);
    if (this->_showEntityDetails)
        this->drawEntityDetails(entityManager, componentManager);

    if (this->_selectedEntity == 0)
        this->drawGuizmo(entityManager, componentManager);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUISystem::drawMenuBar()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit"))
                glfwSetWindowShouldClose(this->_graphic->getWindow().get(), true);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Entity Manager", NULL, &this->_showEntityManager);
            ImGui::MenuItem("Entity Details", NULL, &this->_showEntityDetails);
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

void GUISystem::drawEntityManager(EntityManager &entityManager, ComponentManager &componentManager)
{
    std::size_t size = componentManager.getComponentArray().size();
    ImGui::Begin("Entities", &this->_showEntityManager);

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
                    this->_showEntityDetails = false;
                }
                ImGui::SameLine();
                if (ImGui::Button(std::string("Modify##" + std::to_string(i)).c_str())) {
                    this->_showEntityDetails = true;
                    this->_selectedEntity = i;
                }
            } else {
                if (ImGui::Button(std::string("Create##" + std::to_string(i)).c_str()))
                    entityManager.addMask(i, 0);
            }
        }
        ImGui::EndTable();
        const float footerReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerReserve), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::EndChild();
        ImGui::Separator();
        if (ImGui::Button("Add New Entity"))
            entityManager.addMask(masks.size(), std::nullopt);
    }
    ImGui::End();
}

void GUISystem::drawEntityDetails(EntityManager &entityManager, ComponentManager &componentManager)
{
    ImGui::Begin("Entity Details", &this->_showEntityDetails);
    auto &masks = entityManager.getMasks();

    ImGui::Text("ID: %lu", this->_selectedEntity);
    for (std::size_t i = 0; i < componentManager.getComponentArray().size(); i++) {
        if (masks[this->_selectedEntity].value() & (1 << i)) {
            if (ImGui::CollapsingHeader(FORMAT_NAME(componentManager.getComponentType(i).name()), ImGuiTreeNodeFlags_DefaultOpen)) {
                switch (i) {
                case 0: {
                    Transform &transform = componentManager.getComponent<Transform>(this->_selectedEntity);
                    ImGui::Text("Position:");
                    ImGui::DragFloat3("##pos", (float *)&transform.position, 0.1f, -FLT_MAX, +FLT_MAX);
                    ImGui::Text("Rotation:");
                    ImGui::DragFloat3("##rot", (float *)&transform.rotation, 1.0f, -FLT_MAX, +FLT_MAX);
                    ImGui::Text("Scale:");
                    ImGui::SliderFloat3("##scale", (float *)&transform.scale, 0.0f, 5.0f);
                    break;
                }
                case 1: {
                    Velocity &velocity = componentManager.getComponent<Velocity>(this->_selectedEntity);
                    ImGui::SliderFloat("X##vel", &velocity.x, -10, 10);
                    ImGui::SliderFloat("Y##vel", &velocity.y, -10, 10);
                    ImGui::SliderFloat("Z##vel", &velocity.z, -10, 10);
                    break;
                }
                case 2: {
                    Model &model = componentManager.getComponent<Model>(this->_selectedEntity);
                    const ImU64 increment = 1;
                    ImGui::InputScalar("Model ID", ImGuiDataType_U64, &model.id, &increment);
                    break;
                }
                case 3: {
                    Camera &camera = componentManager.getComponent<Camera>(this->_selectedEntity);
                    const char *names[CameraMod::CAMERA_MOD_COUNT] = {"First", "Third"};

                    ImGui::Text("Forward:");
                    ImGui::DragFloat3("##forward", (float *)&camera.forward, 0.01f, -1.0f, 1.0f);
                    ImGui::SliderInt("Mod", (int *)&camera.mod, 0, CameraMod::CAMERA_MOD_COUNT - 1, names[camera.mod]);
                    ImGui::SliderFloat("FOV", &camera.fov, 30.0f, 90.0f);
                    ImGui::SliderFloat("Near", &camera.near, 0.0f, 100.0f);
                    ImGui::SliderFloat("Far", &camera.far, 0.0f, 1000.0f);
                    ImGui::SliderFloat("Sensitivity", &camera.sensitivity, 0.0f, 1.0f);
                    if (camera.mod == CameraMod::THIRD_PERSON) {
                        ImGui::SliderFloat("Distance", &camera.distance, 0.0f, 100.0f);
                        const ImU64 increment = 1;
                        ImGui::InputScalar("Target ID", ImGuiDataType_U64, &camera.target, &increment);
                        ImGui::Checkbox("align target", &camera.alignTarget);
                    }
                    break;
                }
                case 4: {
                    Light &light = componentManager.getComponent<Light>(this->_selectedEntity);
                    const char *names[LightType::LIGHT_TYPE_COUNT] = {"Dir", "Point", "Spot", "Area"};

                    ImGui::SliderInt("Type", (int *)&light.type, 0, LightType::LIGHT_TYPE_COUNT - 1, names[light.type]);
                    ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 1.0f);
                    ImGui::ColorEdit3("Color", (float *)&light.color);
                    ImGui::SliderFloat("Cut Off", &light.cutOff, 0.0f, 90.0f);
                    break;
                }
                case 5: {
                    Control &control = componentManager.getComponent<Control>(this->_selectedEntity);
                    ImGui::Checkbox("##", &control.control);
                    break;
                }
                case 6: {
                    CubeMap &cubemap = componentManager.getComponent<CubeMap>(this->_selectedEntity);
                    const ImU64 increment = 1;
                    ImGui::InputScalar("CubeMap ID", ImGuiDataType_U64, &cubemap.id, &increment);
                    ImGui::DragFloat("Rotation Speed", &cubemap.rotationSpeed, 0.1f, -FLT_MAX, +FLT_MAX);
                    ImGui::ColorEdit3("Color", (float *)&cubemap.color);
                    break;
                }
                case 7: {
                    BillBoard &bill = componentManager.getComponent<BillBoard>(this->_selectedEntity);
                    const ImU64 increment = 1;
                    ImGui::InputScalar("Texture ID", ImGuiDataType_U64, &bill.textureId, &increment);
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

void GUISystem::drawGuizmo(EntityManager &entityManager, ComponentManager &componentManager)
{
    ImGuizmo::BeginFrame();

    static ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::ROTATE;
    static ImGuizmo::MODE currentGizmoMode = ImGuizmo::WORLD;

    if (ImGui::IsKeyPressed(GLFW_KEY_1))
        currentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(GLFW_KEY_2))
        currentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(GLFW_KEY_3))
        currentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", currentGizmoOperation == ImGuizmo::TRANSLATE))
        currentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", currentGizmoOperation == ImGuizmo::ROTATE))
        currentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", currentGizmoOperation == ImGuizmo::SCALE))
        currentGizmoOperation = ImGuizmo::SCALE;

    if (currentGizmoOperation != ImGuizmo::SCALE) {
        if (ImGui::RadioButton("Local", currentGizmoMode == ImGuizmo::LOCAL))
            currentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", currentGizmoMode == ImGuizmo::WORLD))
            currentGizmoMode = ImGuizmo::WORLD;
    } else
        currentGizmoMode = ImGuizmo::LOCAL;

    glm::mat4 view = this->_graphic->getRenderView().getView();
    glm::mat4 projection = this->_graphic->getRenderView().getProjection();
    Transform &transform = componentManager.getComponent<Transform>(this->_selectedEntity);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model *= glm::toMat4(glm::quat(glm::radians(transform.rotation)));
    model = glm::scale(model, transform.scale);

    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(&view[0][0], &projection[0][0], currentGizmoOperation, currentGizmoMode, &model[0][0], nullptr, nullptr);

    ImGuizmo::DecomposeMatrixToComponents(&model[0][0], &transform.position[0], &transform.rotation[0], &transform.scale[0]);
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
