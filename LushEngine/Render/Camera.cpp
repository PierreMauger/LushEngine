#include "Camera.hpp"

using namespace Lush;

Camera::Camera(float width, float height, std::map<std::string, std::shared_ptr<Shader>> shaders)
{
    this->_view = glm::mat4(1.0f);
    this->_projection = glm::mat4(1.0f);
    this->_position = glm::vec3(0.0f, 0.0f, 10.0f);
    this->_front = glm::vec3(0.0f, 0.0f, -1.0f);
    this->_up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->_yaw = -90.0f;
    this->_pitch = 0.0f;
    this->_fov = 45.0f;
    this->_aspectRatio = width / height;
    this->_near = 0.1f;
    this->_far = 1000.0f;
    this->_projection = glm::perspective(glm::radians(this->_fov), this->_aspectRatio, this->_near, this->_far);
    this->_sensitivity = 0.2f;

    this->_shaders = shaders;
    this->_actShader = nullptr;
}

void Camera::processMouseMovement(float xoffset, float yoffset)
{
    this->_yaw += xoffset * this->_sensitivity;
    this->_pitch += yoffset * this->_sensitivity;

    if (this->_pitch > 89.0f)
        this->_pitch = 89.0f;
    if (this->_pitch < -89.0f)
        this->_pitch = -89.0f;

    glm::vec3 tempFront;
    tempFront.x = cos(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));
    tempFront.y = sin(glm::radians(this->_pitch));
    tempFront.z = sin(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));
    this->_front = glm::normalize(tempFront);
}

void Camera::processKeyboard(Direction dir, float deltaTime)
{
    float speed = deltaTime * 15.0f;

    if (dir == FRONT)
        this->_position += this->_front * speed * 3.0f;
    if (dir == BACK)
        this->_position -= this->_front * speed * 3.0f;
    if (dir == LEFT)
        this->_position -= glm::normalize(glm::cross(this->_front, this->_up)) * speed;
    if (dir == RIGHT)
        this->_position += glm::normalize(glm::cross(this->_front, this->_up)) * speed;
    if (dir == UP)
        this->_position += this->_up * speed;
    if (dir == DOWN)
        this->_position -= this->_up * speed;
}

void Camera::use(std::string shaderName)
{
    if (this->_shaders.find(shaderName) == this->_shaders.end())
        throw std::runtime_error("Using unknown shader: " + shaderName);
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

void Camera::update()
{
    this->_view = glm::lookAt(this->_position, this->_position + this->_front, this->_up);
    this->_projection = glm::perspective(glm::radians(this->_fov), this->_aspectRatio, this->_near, this->_far);
}

void Camera::setView(float time)
{
    this->_actShader->setVec3("viewPos", this->_position);
    this->_actShader->setMat4("view", this->_view);
    this->_actShader->setMat4("projection", this->_projection);
    this->_actShader->setFloat("time", time);
}

void Camera::setDirLight(glm::vec3 direction)
{
    this->_actShader->setVec3("dirLight.direction", direction);
    this->_actShader->setVec3("dirLight.ambient", glm::vec3(1.0f));
    this->_actShader->setVec3("dirLight.diffuse", glm::vec3(1.0f));
    this->_actShader->setVec3("dirLight.specular", glm::vec3(1.0f));
}

void Camera::setPointLights(std::vector<glm::vec3> positions)
{
    this->_actShader->setInt("pointLightCount", positions.size());
    for (int i = 0; i < static_cast<int>(positions.size()) && i < 4; i++) {
        this->_actShader->setVec3("pointLights[" + std::to_string(i) + "].position", positions[i]);
        this->_actShader->setVec3("pointLights[" + std::to_string(i) + "].ambient", glm::vec3(1.0f));
        this->_actShader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", glm::vec3(1.0f));
        this->_actShader->setVec3("pointLights[" + std::to_string(i) + "].specular", glm::vec3(1.0f));
        this->_actShader->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        this->_actShader->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
        this->_actShader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
    }
}

void Camera::setSkyBox()
{
    this->_actShader->setMat4("view", glm::mat4(glm::mat3(this->_view)));
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
        ImGui::SliderFloat("Far", &this->_far, 0.1f, 1000.0f);
        ImGui::SliderFloat("Sensitivity", &this->_sensitivity, 0.1f, 1.0f);
        ImGui::Separator();
        ImGui::Text("Front: %.2f %.2f %.2f", this->_front.x, this->_front.y, this->_front.z);
        ImGui::SliderFloat("PosX", &this->_position.x, -20.0f, 20.0f);
        ImGui::SliderFloat("PosY", &this->_position.y, -20.0f, 20.0f);
        ImGui::SliderFloat("PosZ", &this->_position.z, -20.0f, 20.0f);
    }
    ImGui::End();
}
