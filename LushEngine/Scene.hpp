#ifndef SCENE_HPP
#define SCENE_HPP

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "ECS/Component/Component.hpp"
#include "ECS/Entity/EntityManager.hpp"
#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "RapidXML/rapidxml.hpp"
#include "RapidXML/rapidxml_ext.hpp"
#include "Rendering/Mesh.hpp"
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

            void loadEntity(rapidxml::xml_node<> *node, std::unordered_map<std::string, ScriptClass> &scripts, std::size_t parentId = -1);
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
            static void loadUIElement(rapidxml::xml_node<> *node, Entity &entity);
            static void loadScript(rapidxml::xml_node<> *node, Entity &entity, ScriptClass &script);

            void save(std::unordered_map<std::string, ScriptClass> &scripts);
            void saveEntity(rapidxml::xml_node<> *node, std::unordered_map<std::string, ScriptClass> &scripts, std::size_t entityId, Entity &entity);
            static void saveTransform(rapidxml::xml_node<> *node, Entity &entity);
            static void saveModel(rapidxml::xml_node<> *node, Entity &entity);
            static void saveCamera(rapidxml::xml_node<> *node, Entity &entity);
            static void saveLight(rapidxml::xml_node<> *node, Entity &entity);
            static void saveCubemap(rapidxml::xml_node<> *node, Entity &entity);
            static void saveBillboard(rapidxml::xml_node<> *node, Entity &entity);
            static void saveMap(rapidxml::xml_node<> *node, Entity &entity);
            static void saveRigidBody(rapidxml::xml_node<> *node, Entity &entity);
            static void saveCollider(rapidxml::xml_node<> *node, Entity &entity);
            static void saveCharacterController(rapidxml::xml_node<> *node, Entity &entity);
            static void saveUIElement(rapidxml::xml_node<> *node, Entity &entity);
            static void saveScript(rapidxml::xml_node<> *node, Entity &entity, ScriptClass &script);

            void serialize(boost::archive::binary_iarchive &ar, [[maybe_unused]] const unsigned int version)
            {
                this->_entityManager = std::make_shared<EntityManager>();
                ar &*this->_entityManager;
            }

            void serialize(boost::archive::binary_oarchive &ar, [[maybe_unused]] const unsigned int version)
            {
                ar &*this->_entityManager;
            }
    };
}

#endif // SCENE_HPP
