#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Includes.hpp"
#include "Shader.hpp"
#include "imgui.h"

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
            glm::vec3 _rotation;
            float _fov;
            float _aspectRatio;
            float _near;
            float _far;
            std::map<std::string, std::shared_ptr<Shader>> _shaderMap;
            std::shared_ptr<Shader> _actShader;

        public:
            Camera(float width, float height, std::map<std::string, std::shared_ptr<Shader>> shaderMap);
            ~Camera() = default;

            void use(std::string shaderName);
            std::shared_ptr<Shader> getShader();
            std::shared_ptr<Shader> getShader(std::string shaderName);

            void setShader(float time);
            void setPicking();
            void setOnModel(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);
            void showImGui(bool *open);
    };
}

#endif // CAMERA_HPP
