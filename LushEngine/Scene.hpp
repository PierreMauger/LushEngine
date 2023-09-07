#ifndef SCENE_HPP
#define SCENE_HPP

#include <boost/serialization/shared_ptr.hpp>

#include "ECS/Component/Component.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_print.hpp"
#include "Script/ScriptClass.hpp"

namespace Lush
{
    class Scene : public Resource
    {
        private:
            std::shared_ptr<EntityManager> _entityManager;

        public:
            Scene(File &file, std::unordered_map<std::string, ScriptClass> &scripts);
            Scene() = default;
            ~Scene() = default;

            std::shared_ptr<EntityManager> getEntityManager();

            void load(File &file, std::unordered_map<std::string, ScriptClass> &scripts);
            void reload(File &file, std::unordered_map<std::string, ScriptClass> &scripts);

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &this->_entityManager;
            }
    };
}

#endif // SCENE_HPP
