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

Camera ::~Camera()
{
}
