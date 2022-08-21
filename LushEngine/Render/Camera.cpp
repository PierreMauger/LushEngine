#include "Camera.hpp"

using namespace Lush;

Camera::Camera(float width, float height, std::map<std::string, std::shared_ptr<Shader>> shaderMap)
{
    this->_view = glm::mat4(1.0f);
    this->_projection = glm::mat4(1.0f);
    this->_position = glm::vec3(10.0f, 0.0f, 5.0f);
    this->_front = glm::vec3(0.0f, 0.0f, 0.0f);
    this->_rotation = glm::vec3(0.0f);
    this->_up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->_fov = 45.0f;
    this->_aspectRatio = width / height;
    this->_near = 0.1f;
    this->_far = 100.0f;
    this->_projection = glm::perspective(glm::radians(this->_fov), this->_aspectRatio, this->_near, this->_far);

    this->_shaders = shaderMap;
    this->_actShader = nullptr;
}

void Camera::use(std::string shaderName)
{
    this->_shaders[shaderName]->use();
    this->_actShader = this->_shaders[shaderName];
}

std::shared_ptr<Shader> Camera::getShader()
{
    return this->_actShader;
}

std::shared_ptr<Shader> Camera::getShader(std::string shaderName)
{
    if (this->_shaders.find(shaderName) == this->_shaders.end())
        throw std::runtime_error("Getting a shader not loaded: " + shaderName);
    return this->_shaders[shaderName];
}

void Camera::setShader(float time)
{
    this->_front = -this->_position;
    this->_view = glm::lookAt(this->_position, glm::vec3(0.0f), this->_up);
    this->_projection = glm::perspective(glm::radians(this->_fov), this->_aspectRatio, this->_near, this->_far);

    this->_actShader->setVec3("viewPos", this->_position);
    this->_actShader->setVec3("lightPos", this->_position);
    this->_actShader->setVec3("lightColor", glm::vec3(1.0f));
    this->_actShader->setMat4("view", this->_view);
    this->_actShader->setMat4("projection", this->_projection);
    this->_actShader->setFloat("time", time);
}

void Camera::setPicking()
{
    // assume setShader has been called, so _view and _projection are set
    this->_actShader->setVec3("viewPos", this->_position);
    this->_actShader->setMat4("view", this->_view);
    this->_actShader->setMat4("projection", this->_projection);
}

void Camera::setOnModel(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, scale);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    this->_actShader->setMat4("model", model);

    for (std::size_t i = 0; i < 100; i++)
        this->_actShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", glm::mat4(1.0f));
}

void Camera::showImGui(bool *open)
{
    if (ImGui::Begin("Camera", open)) {
        ImGui::SliderFloat("FOV", &this->_fov, 30.0f, 90.0f);
        ImGui::SliderFloat("Aspect Ratio", &this->_aspectRatio, 0.5f, 2.0f);
        ImGui::SliderFloat("Near", &this->_near, 0.1f, 10.0f);
        ImGui::SliderFloat("Far", &this->_far, 0.1f, 100.0f);
        ImGui::Text(" ");
        ImGui::SliderFloat("PosX", &this->_position.x, -20.0f, 20.0f);
        ImGui::SliderFloat("PosY", &this->_position.y, -20.0f, 20.0f);
        ImGui::SliderFloat("PosZ", &this->_position.z, -20.0f, 20.0f);
    }
    ImGui::End();
}
