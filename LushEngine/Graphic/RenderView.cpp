#include "RenderView.hpp"

using namespace Lush;

RenderView::RenderView(float width, float height)
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
    this->_far = 100.0f;
    this->_sensitivity = 0.2f;

    this->_projection = glm::perspective(glm::radians(this->_fov), this->_aspectRatio, this->_near, this->_far);
}

void RenderView::setShaders(std::map<std::string, Shader> &shaders)
{
    this->_shaders = shaders;
}

Shader &RenderView::getShader()
{
    return this->_shaders[this->_actShader];
}

Shader &RenderView::getShader(std::string shaderName)
{
    if (this->_shaders.find(shaderName) == this->_shaders.end())
        throw std::runtime_error("Getting a shader not loaded: " + shaderName);
    return this->_shaders[shaderName];
}

void RenderView::use(std::string shaderName)
{
    if (this->_shaders.find(shaderName) == this->_shaders.end())
        throw std::runtime_error("Using unknown shader: " + shaderName);
    this->_shaders[shaderName].use();
    this->_actShader = shaderName;
}

void RenderView::update(Transform transform, Camera camera)
{
    this->_position = transform.position;

    this->_yaw = transform.rotation.y;
    this->_pitch = transform.rotation.x;

    this->_front.x = cos(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));
    this->_front.y = sin(glm::radians(this->_pitch));
    this->_front.z = sin(glm::radians(this->_yaw)) * cos(glm::radians(this->_pitch));

    this->_fov = camera.fov;
    this->_near = camera.near;
    this->_far = camera.far;
    this->_sensitivity = camera.sensitivity;

    this->_view = glm::lookAt(this->_position, this->_position + this->_front, this->_up);
    this->_projection = glm::perspective(glm::radians(this->_fov), this->_aspectRatio, this->_near, this->_far);
}

void RenderView::setView(float time)
{
    this->_shaders[this->_actShader].setVec3("viewPos", this->_position);
    this->_shaders[this->_actShader].setMat4("view", this->_view);
    this->_shaders[this->_actShader].setMat4("projection", this->_projection);
    this->_shaders[this->_actShader].setFloat("time", time);
}

void RenderView::setDirLight(std::pair<Transform, Light> dirLight)
{
    this->_shaders[this->_actShader].setVec3("dirLight.direction", dirLight.first.rotation);
    this->_shaders[this->_actShader].setVec3("dirLight.ambient", dirLight.second.color);
    this->_shaders[this->_actShader].setVec3("dirLight.diffuse", dirLight.second.color);
    this->_shaders[this->_actShader].setVec3("dirLight.specular", dirLight.second.color);
}

void RenderView::setPointLights(std::vector<std::pair<Transform, Light>> pointLights)
{
    this->_shaders[this->_actShader].setInt("pointLightCount", pointLights.size());
    for (std::size_t i = 0; i < pointLights.size() && i < 4; i++) {
        this->_shaders[this->_actShader].setVec3("pointLights[" + std::to_string(i) + "].position", pointLights[i].first.position);
        this->_shaders[this->_actShader].setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLights[i].second.color);
        this->_shaders[this->_actShader].setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLights[i].second.color);
        this->_shaders[this->_actShader].setVec3("pointLights[" + std::to_string(i) + "].specular", pointLights[i].second.color);
        this->_shaders[this->_actShader].setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        this->_shaders[this->_actShader].setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
        this->_shaders[this->_actShader].setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
    }
}

void RenderView::setSkyBox()
{
    this->_shaders[this->_actShader].setMat4("view", glm::mat4(glm::mat3(this->_view))); // remove translation from the view matrix
    this->_shaders[this->_actShader].setMat4("projection", this->_projection);
}

void RenderView::setOnModel(Transform &transform)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, transform.scale);
    this->_shaders[this->_actShader].setMat4("model", model);

    for (std::size_t i = 0; i < 100; i++)
        this->_shaders[this->_actShader].setMat4("finalBonesMatrices[" + std::to_string(i) + "]", glm::mat4(1.0f));
    // TODO animator system ?
}
