#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Includes.hpp"
#include "Systems/Render/Shader.hpp"
#include "ComponentTypes.hpp"

namespace Lush
{
    class Camera
    {
        private:
            glm::mat4 _view;
            glm::mat4 _projection;
            glm::vec3 _position;
            glm::vec3 _front;
            glm::vec3 _up;
            float _yaw;
            float _pitch;
            float _fov;
            float _aspectRatio;
            float _near;
            float _far;
            float _sensitivity;
            std::map<std::string, std::shared_ptr<Shader>> _shaders;
            std::shared_ptr<Shader> _actShader;

        public:
            Camera(float width, float height);
            ~Camera() = default;

            void setShaders(std::map<std::string, std::shared_ptr<Shader>> &shaders);

            void processMouseMovement(float xoffset, float yoffset);

            void use(std::string shaderName);
            std::shared_ptr<Shader> getShader();
            std::shared_ptr<Shader> getShader(std::string shaderName);

            void update(Transform transform, CameraComponent camera);
            void setView(float time);
            void setDirLight(glm::vec3 dir);
            void setPointLights(std::vector<glm::vec3> pointLights);
            void setSkyBox();
            void setOnModel(Transform &transform);
            void setOnModel(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    };
}

#endif // CAMERA_HPP
