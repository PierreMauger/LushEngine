#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ComponentTypes.hpp"
#include "ECS/ECS.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "Systems/Camera/CameraSystem.hpp"
#include "Systems/FileWatcher/FileWatcherSystem.hpp"
#include "Systems/GUI/GUISystem.hpp"
#include "Systems/Picking/PickingSystem.hpp"
#include "Systems/Render/RenderSystem.hpp"
#include "Systems/Scene/SceneSystem.hpp"
#include "Systems/Script/ScriptSystem.hpp"

namespace Lush
{
    class Engine
    {
        private:
            ECS _ecs;
            std::shared_ptr<Graphic> _graphic;
            float _lastTime = 0.0f;
            float _deltaTime = 0.0f;

        public:
            Engine();
            ~Engine() = default;

            void run();
            void updateMouse();
    };
}

#endif // ENGINE_HPP
