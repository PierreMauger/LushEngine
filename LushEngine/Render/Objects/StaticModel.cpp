#include "StaticModel.hpp"

using namespace Lush;

StaticModel::StaticModel(GameObject obj, std::shared_ptr<Model> model) : RenderObject(obj)
{
    this->_model = model;
}

void StaticModel::draw(Camera &camera)
{
    if (this->_isHovered) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // enable stencil writing
        glStencilMask(0xFF);
    }

    if (this->_polygonMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    camera.getShader().setBool("outline", false);
    camera.getShader().setBool("isSelected", this->_isSelected);
    camera.setOnModel(this->_position, this->_scale, this->_rotation);
    this->_model->draw(camera.getShader());

    if (this->_isHovered) {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        camera.setOnModel(this->_position, this->_scale * glm::vec3(1.05f), this->_rotation);
        camera.getShader().setBool("outline", true);
        this->_model->draw(camera.getShader());
        glStencilFunc(GL_ALWAYS, 0, 0xFF);  // disable stencil writing
        glStencilMask(0xFF);
    }
}

void StaticModel::showImGui(int id)
{
    if (ImGui::Begin((this->_name + std::to_string(id)).c_str(), &this->_isSelected)) {
        ImGui::Text("Position: %f, %f, %f", this->_position.x, this->_position.y, this->_position.z);
        ImGui::Text("Scale: %f, %f, %f", this->_scale.x, this->_scale.y, this->_scale.z);
        ImGui::Text("Rotation: %f, %f, %f", this->_rotation.x, this->_rotation.y, this->_rotation.z);
        ImGui::Text(" ");
        ImGui::Checkbox("Is Hovered", &this->_isHovered);
        ImGui::Checkbox("Is Selected", &this->_isSelected);
        ImGui::Checkbox("Polygon Mode", &this->_polygonMode);
    }
    ImGui::End();
}
