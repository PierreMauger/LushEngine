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
            Shader _shader;
            Shader _picking;

        public:
            Camera(float width, float height);
            ~Camera() = default;
            Shader &getShader();
            Shader &getPicking();
            void setShader(float time);
            void setPicking();
            void setOnModel(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);
            void showImGui(bool *open);
            glm::mat4 getView() const;
    };
}

#endif // CAMERA_HPP
