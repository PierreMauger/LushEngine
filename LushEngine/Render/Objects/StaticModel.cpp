#include "StaticModel.hpp"

using namespace Lush;

StaticModel::StaticModel(GameObject obj, std::shared_ptr<Model> model) : RenderObject(obj)
{
    this->_model = model;
}

void StaticModel::draw(Camera &camera)
{
    if (this->_polygonMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    camera.setOnModel(this->_position, this->_scale, this->_rotation);
    this->_model->draw(*camera.getShader());
}

bool StaticModel::showImGui(int id)
{
    bool positionChanged = false;

    if (ImGui::Begin((this->_name + "." + std::to_string(id)).c_str(), &this->_isSelected)) {
        ImGui::Text("Position: %.2f, %.2f, %.2f", this->_position.x, this->_position.y, this->_position.z);
        ImGui::Text("Scale: %.2f, %.2f, %.2f", this->_scale.x, this->_scale.y, this->_scale.z);
        ImGui::Text("Rotation: %.2f, %.2f, %.2f", this->_rotation.x, this->_rotation.y, this->_rotation.z);
        ImGui::Text(" ");
        ImGui::Checkbox("Is Hovered", &this->_isHovered);
        ImGui::Checkbox("Is Selected", &this->_isSelected);
        ImGui::Checkbox("Polygon Mode", &this->_polygonMode);
        ImGui::Text(" ");
        if (ImGui::SliderFloat("PosX", &this->_position.x, -5.0f, 5.0f))
            positionChanged = true;
        if (ImGui::SliderFloat("PosY", &this->_position.y, -5.0f, 5.0f))
            positionChanged = true;
        if (ImGui::SliderFloat("PosZ", &this->_position.z, -5.0f, 5.0f))
            positionChanged = true;
    }
    ImGui::End();
    return positionChanged;
}
