#include "Camera.hpp"

using namespace Lush;

Camera::Camera(float width, float height) : _shader("resources/shaders/camera.vs", "resources/shaders/camera.fs")
{
    this->_view = glm::mat4(1.0f);
    this->_projection = glm::mat4(1.0f);
    this->_position = glm::vec3(10.0f, 0.0f, 0.0f);
    this->_front = glm::vec3(0.0f, 0.0f, 0.0f);
    this->_rotation = glm::vec3(0.0f);
    this->_up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->_fov = 45.0f;
    this->_aspectRatio = width / height;
    this->_near = 0.1f;
    this->_far = 100.0f;
    this->_projection = glm::perspective(glm::radians(this->_fov), this->_aspectRatio, this->_near, this->_far);
}

Shader &Camera::getShader()
{
    return this->_shader;
}

void Camera::setShader(float time)
{
    this->_front = -this->_position;
    this->_view = glm::lookAt(this->_position, glm::vec3(0.0f), this->_up);

    this->_shader.setVec3("viewPos", this->_position);
    this->_shader.setVec3("lightPos", this->_position);
    this->_shader.setVec3("lightColor", glm::vec3(1.0f));
    this->_shader.setMat4("view", this->_view);
    this->_shader.setMat4("projection", this->_projection);
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
