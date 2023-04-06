#ifndef RENDERVIEW_HPP
#define RENDERVIEW_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ComponentTypes.hpp"
#include "Includes.hpp"
#include "Rendering/Shader.hpp"

namespace Lush
{
    class RenderView
    {
        private:
            glm::mat4 _view;
            glm::mat4 _projection;
            glm::vec3 _position;
            glm::vec3 _front;
            glm::vec3 _up;
            float _aspectRatio;
            float _fov;
            float _near;
            float _far;
            float _sensitivity;
            std::unordered_map<std::string, Shader> _shaders;
            std::string _actShader;

        public:
            RenderView(float aspectRatio);
            ~RenderView() = default;

            void setAspectRatio(float aspectRatio);
            void setShaders(std::unordered_map<std::string, Shader> &shaders);
            Shader &getShader();
            Shader &getShader(std::string shaderName);

            glm::mat4 getView();
            glm::mat4 getProjection();

            void use(std::string shaderName);
            void update(Transform transform, Camera camera);
            void rotate(Transform &transform, glm::vec2 offset) const;
            void setView();
            void setSkyBoxView();
            void setDirLights(std::vector<std::pair<Transform, Light>> dirLights);
            void setPointLights(std::vector<std::pair<Transform, Light>> pointLights);
            void setModel(Transform transform);
            void setBillboard(Transform transform);
    };
}

#endif // RENDERVIEW_HPP
