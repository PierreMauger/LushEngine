#ifndef SCENE_HPP
#define SCENE_HPP

#include "ECS/Component/Component.hpp"
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

        public:
            Scene(File &file, std::unordered_map<std::string, ScriptClass> &scripts);
            Scene() = default;
            ~Scene() = default;

            EntityManager &getEntityManager();

            void load(File &file, std::unordered_map<std::string, ScriptClass> &scripts);
            void reload(File &file, std::unordered_map<std::string, ScriptClass> &scripts);

            void setScene(EntityManager &entityManager);

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &_entityManager;
            }
    };
}

#endif // SCENE_HPP
