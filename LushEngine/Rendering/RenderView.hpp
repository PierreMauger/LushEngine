#ifndef RENDER_VIEW_HPP
#define RENDER_VIEW_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ECS/Component/Component.hpp"
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
            CameraType _type;
            float _aspectRatio;
            float _fov;
            float _near;
            float _far;
            float _sensitivity;
            std::unordered_map<std::string, std::shared_ptr<Shader>> _shaders;
            std::string _actShader;

        public:
            RenderView(float aspectRatio);
            ~RenderView() = default;

            void setAspectRatio(float aspectRatio);
            void setShaders(std::unordered_map<std::string, std::shared_ptr<Shader>> &shaders);
            Shader &getShader();
            Shader &getShader(std::string shaderName);

            glm::mat4 getView();
            glm::mat4 getProjection();

            void setLightMatrix();
            glm::mat4 getLightMatrix();

            void use(std::string shaderName);
            void update(Transform transform, Camera camera);
            void rotate(Transform &transform, glm::vec2 offset) const;
            void setView();
            void setSkyboxView();
            void setDirLights(std::vector<std::pair<Transform, Light>> dirLights);
            void setPointLights(std::vector<std::pair<Transform, Light>> pointLights);
            void setModel(Transform transform);
            void setBillboard(Transform transform);
    };
}

#endif // RENDER_VIEW_HPP
