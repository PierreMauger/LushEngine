#include "Rendering/RenderView.hpp"

using namespace Lush;

static glm::mat4 lightSpaceMatrix;

RenderView::RenderView(float aspectRatio)
{
    this->_view = glm::mat4(1.0f);
    this->_projection = glm::mat4(1.0f);
    this->_position = glm::vec3(0.0f, 0.0f, 10.0f);
    this->_front = glm::vec3(0.0f, 0.0f, -1.0f);
    this->_up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->_aspectRatio = aspectRatio;
    this->_fov = 45.0f;
    this->_near = 0.1f;
    this->_far = 100.0f;
    this->_sensitivity = 0.1f;

    this->_projection = glm::perspective(glm::radians(this->_fov), this->_aspectRatio, this->_near, this->_far);
}

void RenderView::setAspectRatio(float aspectRatio)
{
    this->_aspectRatio = aspectRatio;
    this->_projection = glm::perspective(glm::radians(this->_fov), this->_aspectRatio, this->_near, this->_far);
}

void RenderView::setShaders(std::unordered_map<std::string, std::shared_ptr<Shader>> &shaders)
{
    this->_shaders = shaders;
}

Shader &RenderView::getShader()
{
    return *this->_shaders[this->_actShader];
}

Shader &RenderView::getShader(std::string shaderName)
{
    if (!this->_shaders.contains(shaderName))
        throw std::runtime_error("Getting a shader not loaded: " + shaderName);
    return *this->_shaders[shaderName];
}

glm::mat4 RenderView::getView()
{
    return this->_view;
}

glm::mat4 RenderView::getProjection()
{
    return this->_projection;
}

void RenderView::setLightMatrix()
{
    lightSpaceMatrix = this->_projection * this->_view;
}

glm::mat4 RenderView::getLightMatrix()
{
    return lightSpaceMatrix;
}

void RenderView::use(std::string shaderName)
{
    if (!this->_shaders.contains(shaderName))
        throw std::runtime_error("Using unknown shader: " + shaderName);
    if (this->_actShader != shaderName) {
        this->_actShader = shaderName;
        this->_shaders[shaderName]->use();
    }
}

void RenderView::update(Transform transform, Camera camera)
{
    this->_position = transform.position;
    this->_front = camera.forward;

    this->_fov = camera.fov;
    this->_near = camera.near;
    this->_far = camera.far;
    this->_type = camera.type;

    if (this->_type == CameraType::PERSPECTIVE)
        this->_projection = glm::perspective(glm::radians(this->_fov), this->_aspectRatio, this->_near, this->_far);
    else if (this->_type == CameraType::ORTHOGRAPHIC)
        this->_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, this->_near, this->_far);

    this->_view = glm::lookAt(this->_position, this->_position + this->_front, this->_up);
}

void RenderView::rotate(Transform &transform, glm::vec2 offset) const
{
    // x is yaw, y is pitch
    transform.rotation.x += offset.x * this->_sensitivity;
    transform.rotation.y += offset.y * this->_sensitivity;

    transform.rotation.x = fmod(transform.rotation.x, 360.0f);
    transform.rotation.y = glm::clamp(transform.rotation.y, -89.0f, 89.0f);
}

void RenderView::setView()
{
    this->_shaders[this->_actShader]->setVec3("viewPos", this->_position);
    this->_shaders[this->_actShader]->setMat4("view", this->_view);
    this->_shaders[this->_actShader]->setMat4("projection", this->_projection);
    this->_shaders[this->_actShader]->setMat4("lightSpaceMatrix", lightSpaceMatrix);
}

void RenderView::setSkyboxView()
{
    this->_shaders[this->_actShader]->setMat4("view", glm::mat4(glm::mat3(this->_view))); // remove translation from the view matrix
    this->_shaders[this->_actShader]->setMat4("projection", this->_projection);
}

void RenderView::setDirLights(std::vector<std::pair<Transform, Light>> dirLights)
{
    this->_shaders[this->_actShader]->setInt("dirLightCount", (int)dirLights.size());

    for (std::size_t i = 0; i < dirLights.size() && i < 2; i++) {
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::quat q = glm::quat(glm::radians(dirLights[i].first.rotation));
        direction = glm::mat3(glm::toMat4(q)) * direction;

        this->_shaders[this->_actShader]->setVec3("dirLights[" + std::to_string(i) + "].direction", direction);
        this->_shaders[this->_actShader]->setVec3("dirLights[" + std::to_string(i) + "].ambient", dirLights[i].second.color);
        this->_shaders[this->_actShader]->setVec3("dirLights[" + std::to_string(i) + "].diffuse", dirLights[i].second.color);
        this->_shaders[this->_actShader]->setVec3("dirLights[" + std::to_string(i) + "].specular", dirLights[i].second.color);
    }
}

void RenderView::setPointLights(std::vector<std::pair<Transform, Light>> pointLights)
{
    this->_shaders[this->_actShader]->setInt("pointLightCount", (int)pointLights.size());
    for (std::size_t i = 0; i < pointLights.size() && i < 4; i++) {
        this->_shaders[this->_actShader]->setVec3("pointLights[" + std::to_string(i) + "].position", pointLights[i].first.position);
        this->_shaders[this->_actShader]->setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLights[i].second.color);
        this->_shaders[this->_actShader]->setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLights[i].second.color);
        this->_shaders[this->_actShader]->setVec3("pointLights[" + std::to_string(i) + "].specular", pointLights[i].second.color);
        this->_shaders[this->_actShader]->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        this->_shaders[this->_actShader]->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
        this->_shaders[this->_actShader]->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
    }
}

void RenderView::setModel(Transform transform)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model *= glm::toMat4(glm::quat(glm::radians(transform.rotation)));
    model = glm::scale(model, transform.scale);

    this->_shaders[this->_actShader]->setMat4("model", model);
}

void RenderView::setBillboard(Transform transform)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, transform.position);
    model = glm::scale(model, transform.scale);

    this->_shaders[this->_actShader]->setMat4("model", model);
}
