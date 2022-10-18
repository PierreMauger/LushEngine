#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ComponentTypes.hpp"
#include "ECS/System/ISystem.hpp"
#include "Includes.hpp"
#include "Systems/Render/Camera.hpp"
#include "Systems/Render/Model.hpp"
#include "Systems/Render/Shader.hpp"

namespace Lush
{
    class RenderSystem : public ISystem
    {
        private:
            std::shared_ptr<GLFWwindow> _window;
            std::map<std::string, std::shared_ptr<Shader>> _shaders;
            std::map<std::size_t, std::shared_ptr<Model>> _models;
            Camera _camera;

            std::vector<std::pair<Transform, Light>> _dirLights;
            std::vector<std::pair<Transform, Light>> _pointLights;
            std::vector<std::pair<Transform, Light>> _spotLights;
            // std::vector<Light> _areaLights;

        public:
            RenderSystem(std::shared_ptr<GLFWwindow> window);
            ~RenderSystem();

            void update(ComponentManager &componentManager, EntityManager &entityManager);
    };
}

#endif // RENDERSYSTEM_HPP
