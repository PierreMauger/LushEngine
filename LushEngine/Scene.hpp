#ifndef SCENE_HPP
#define SCENE_HPP

#include "File/File.hpp"
#include "Includes.hpp"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "ComponentTypes.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "ECS/Component/ComponentManager.hpp"

namespace Lush
{
    class Scene
    {
        private:
            EntityManager _entityManager;
            ComponentManager _componentManager;

        public:
            Scene(File &file);
            ~Scene() = default;

            void load(File &file);

            void setScene(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // SCENE_HPP
