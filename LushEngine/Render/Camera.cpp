#include "Camera.hpp"

using namespace Lush;

Camera::Camera() : _shader("resources/shaders/camera.vs", "resources/shaders/camera.fs")
{
    this->_view = glm::mat4(1.0f);
    this->_projection = glm::mat4(1.0f);
    this->_position = glm::vec3(0.0f);
    this->_rotation = glm::vec3(0.0f);
    this->_up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->_fov = 45.0f;
    this->_aspectRatio = 1.0f;
    this->_near = 0.1f;
    this->_far = 100.0f;
}

Shader &Camera::getShader()
{
    return this->_shader;
}

void Camera::setShader(float time)
{
    this->_shader.setFloat("time", time);
}

void Camera::setOnModel(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::scale(model, scale);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    this->_shader.setMat4("model", model);

    for (std::size_t i = 0; i < 100; i++)
        this->_shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", glm::mat4(1.0f));
}
