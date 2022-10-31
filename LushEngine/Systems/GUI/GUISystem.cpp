#include "Systems/GUI/GUISystem.hpp"

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

    if (this->_showEntityManager)
        this->drawEntityManager(entityManager, componentManager);
    if (this->_showEntityDetails)
        this->drawEntityDetails(entityManager, componentManager);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
                    ImGui::SliderFloat("FOV", &camera.fov, 30.0f, 90.0f);
                    ImGui::SliderFloat("Near", &camera.near, 0.0f, 100.0f);
                    ImGui::SliderFloat("Far", &camera.far, 0.0f, 1000.0f);
                    ImGui::SliderFloat("Sensitivity", &camera.sensitivity, 0.0f, 1.0f);
                    break;
                }
                case 4: {
                    Light &light = componentManager.getComponent<Light>(this->_selectedEntity);
                    const char* names[LightType::LIGHT_TYPE_COUNT] = { "Dir", "Point", "Spot", "Area" };

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
