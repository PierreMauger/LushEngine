#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ECS/Component/Component.hpp"
#include "ECS/ECS.hpp"
#include "Graphic.hpp"
#include "Includes.hpp"
#include "ResourceManager.hpp"
#include "Systems/Camera/CameraSystem.hpp"
#include "Systems/FileWatcher/FileWatcherSystem.hpp"
#include "Systems/GUI/GUISystem.hpp"
#include "Systems/Game/GameSystem.hpp"
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
            std::shared_ptr<ResourceManager> _resourceManager;
            float _lastTime = 0.0f;
            float _deltaTime = 0.0f;
            bool _isEditor = false;

        public:
            explicit Engine(bool isEditor = false);
            ~Engine() = default;

            void run();
            void updateMouse();
    };
}

#endif // ENGINE_HPP
