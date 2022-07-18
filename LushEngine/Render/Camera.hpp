#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Includes.hpp"
#include "Shader.hpp"

namespace Lush
{
    class Camera
    {
        private:
            glm::mat4 _view;
            glm::mat4 _projection;
            glm::vec3 _position;
            glm::vec3 _rotation;
            glm::vec3 _up;
            float _fov;
            float _aspectRatio;
            float _near;
            float _far;
            Shader _shader;

        public:
            Camera();
            ~Camera();
    };
}

#endif // CAMERA_HPP
