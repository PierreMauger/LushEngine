#ifndef SCENE_HPP
#define SCENE_HPP

#include "ComponentTypes.hpp"
#include "ECS/Component/ComponentManager.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "File/File.hpp"
#include "Includes.hpp"
#include "Script/ScriptClass.hpp"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

namespace Lush
{
    class Scene : public Resource
    {
        private:
            EntityManager _entityManager;
            ComponentManager _componentManager;

        public:
            Scene(File &file, std::unordered_map<std::string, ScriptClass> &scripts);
            ~Scene() = default;

            void load(File &file, std::unordered_map<std::string, ScriptClass> &scripts);

            void setScene(EntityManager &entityManager, ComponentManager &componentManager);
    };
}

#endif // SCENE_HPP
