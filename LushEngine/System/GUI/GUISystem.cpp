#include "System/GUI/GUISystem.hpp"

using namespace Lush;

GUISystem::GUISystem(std::shared_ptr<GLFWwindow> window)
{
    this->_window = window;

    if (!IMGUI_CHECKVERSION())
        throw std::runtime_error("IMGUI version is invalid");
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(this->_window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

GUISystem::~GUISystem()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUISystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (this->_showEntityManager)
        this->drawEntityManager(componentManager, entityManager);
    if (this->_showEntityDetails)
        this->drawEntityDetails(componentManager, entityManager);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUISystem::drawEntityManager(ComponentManager &componentManager, EntityManager &entityManager)
{
    std::size_t size = componentManager.getComponentArray().size();
    ImGui::Begin("Entities", &this->_showEntityManager);

    if (ImGui::BeginTable("Entities", 3)) {
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

void GUISystem::drawEntityDetails(ComponentManager &componentManager, EntityManager &entityManager)
{
    ImGui::Begin("Entity Details", &this->_showEntityDetails);
    auto &masks = entityManager.getMasks();

    for (std::size_t i = 0; i < componentManager.getComponentArray().size(); i++) {
        if (masks[this->_selectedEntity].value() & (1 << i)) {
            if (ImGui::CollapsingHeader(componentManager.getComponentType(i).name() + std::strlen(componentManager.getComponentType(i).name()) / 10 + 1)) {
                // TODO: Add a way to draw and modify components
            }
        }
    }

    const float footerReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerReserve), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::EndChild();
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Add Component")) {
        for (std::size_t i = 0; i < componentManager.getComponentArray().size(); i++) {
            if (!(masks[this->_selectedEntity].value() & (1 << i))) {
                if (ImGui::Selectable(componentManager.getComponentType(i).name() + std::strlen(componentManager.getComponentType(i).name()) / 10 + 1)) {
                    entityManager.updateMask(this->_selectedEntity, masks[this->_selectedEntity].value() | (1 << i));
                    // TODO: fill component with default values
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
