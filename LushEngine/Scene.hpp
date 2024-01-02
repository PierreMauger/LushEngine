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

            void load(const File &file, std::unordered_map<std::string, ScriptClass> &scripts);
            void reload(const File &file, std::unordered_map<std::string, ScriptClass> &scripts);

            static void loadTransform(rapidxml::xml_node<> *node, Entity &entity);
            static void loadModel(rapidxml::xml_node<> *node, Entity &entity);
            static void loadCamera(rapidxml::xml_node<> *node, Entity &entity);
            static void loadLight(rapidxml::xml_node<> *node, Entity &entity);
            static void loadCubemap(rapidxml::xml_node<> *node, Entity &entity);
            static void loadBillboard(rapidxml::xml_node<> *node, Entity &entity);
            static void loadMap(rapidxml::xml_node<> *node, Entity &entity);
            static void loadRigidBody(rapidxml::xml_node<> *node, Entity &entity);
            static void loadCollider(rapidxml::xml_node<> *node, Entity &entity);
            static void loadCharacterController(rapidxml::xml_node<> *node, Entity &entity);
            void loadScript(rapidxml::xml_node<> *node, Entity &entity, ScriptClass &script);

            template <class Archive> void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &*this->_entityManager;
            }
    };
}

#endif // SCENE_HPP
