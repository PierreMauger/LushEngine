#include "Scene.hpp"

using namespace Lush;

Scene::Scene(File &file, std::map<std::string, ScriptClass> &scripts)
{
    this->_componentManager.bindComponent<Transform>();
    this->_componentManager.bindComponent<Velocity>();
    this->_componentManager.bindComponent<Model>();
    this->_componentManager.bindComponent<Camera>();
    this->_componentManager.bindComponent<Light>();
    this->_componentManager.bindComponent<Cubemap>();
    this->_componentManager.bindComponent<Billboard>();
    this->_componentManager.bindComponent<Map>();

    this->load(file, scripts);
}

void Scene::load(File &file, std::map<std::string, ScriptClass> &scripts)
{
    rapidxml::xml_document<> doc;
    std::string xml = file.load();
    char *xmlCopy = new char[file.load().size() + 1];

    std::strcpy(xmlCopy, xml.c_str());
    doc.parse<0>(xmlCopy);

    rapidxml::xml_node<> *rootNode = doc.first_node("Scene");
    rapidxml::xml_node<> *entitiesNode = rootNode->first_node("Entities");
    for (rapidxml::xml_node<> *entityNode = entitiesNode->first_node("Entity"); entityNode; entityNode = entityNode->next_sibling("Entity")) {
        int id = std::atoi(entityNode->first_attribute("id")->value());
        int mask = 0;
        this->_entityManager.addMask(id, mask);

        rapidxml::xml_node<> *componentsNode = entityNode->first_node("Components");
        for (rapidxml::xml_node<> *componentNode = componentsNode->first_node(); componentNode; componentNode = componentNode->next_sibling()) {
            std::string name = componentNode->name();
            if (name == "Transform") {
                mask |= ComponentType::TRANSFORM;
                Transform temp;
                std::sscanf(componentNode->first_attribute("position")->value(), "%f %f %f", &temp.position.x, &temp.position.y, &temp.position.z);
                std::sscanf(componentNode->first_attribute("rotation")->value(), "%f %f %f", &temp.rotation.x, &temp.rotation.y, &temp.rotation.z);
                std::sscanf(componentNode->first_attribute("scale")->value(), "%f %f %f", &temp.scale.x, &temp.scale.y, &temp.scale.z);
                this->_componentManager.addComponent<Transform>(id, temp);
            } else if (name == "Velocity") {
                mask |= ComponentType::VELOCITY;
                // attributs
                this->_componentManager.addComponent<Velocity>(id);
            } else if (name == "Model") {
                mask |= ComponentType::MODEL;
                Model temp;
                temp.name = componentNode->first_attribute("name")->value();
                this->_componentManager.addComponent<Model>(id, temp);
            } else if (name == "Camera") {
                mask |= ComponentType::CAMERA;
                Camera temp;
                // attributs
                this->_componentManager.addComponent<Camera>(id, temp);
            } else if (name == "Light") {
                mask |= ComponentType::LIGHT;
                Light temp;
                // attributs
                this->_componentManager.addComponent<Light>(id, temp);
            } else if (name == "Cubemap") {
                mask |= ComponentType::CUBEMAP;
                Cubemap temp;
                temp.name = componentNode->first_attribute("name")->value();
                this->_componentManager.addComponent<Cubemap>(id, temp);
            } else if (name == "Billboard") {
                mask |= ComponentType::BILLBOARD;
                Billboard temp;
                temp.name = componentNode->first_attribute("name")->value();
                this->_componentManager.addComponent<Billboard>(id, temp);
            } else if (name == "Map") {
                mask |= ComponentType::MAP;
                Map temp;
                temp.name = componentNode->first_attribute("name")->value();
                this->_componentManager.addComponent<Map>(id, temp);
            }
            std::size_t it = 0;
            for (auto &[scriptName, script] : scripts) {
                if (scriptName == name) {
                    mask |= ComponentType::COMPONENT_TYPE_COUNT << it;
                    std::map<std::string, std::any> fieldsValues;
                    for (auto &[fieldName, field] : script.getFields()) {
                        if (field.type == "Single")
                            fieldsValues[fieldName] = 0.0f;
                        if (field.type == "Entity" || field.type == "UInt64")
                            fieldsValues[fieldName] = (unsigned long)0;
                    }
                    for (rapidxml::xml_attribute<> *attribute = componentNode->first_attribute(); attribute; attribute = attribute->next_attribute()) {
                        std::string attributeName = attribute->name();
                        if (scripts[scriptName].getFields().find(attributeName) != scripts[scriptName].getFields().end()) {
                            if (scripts[scriptName].getFields()[attributeName].type == "Single")
                                fieldsValues[attributeName] = std::stof(attribute->value());
                            if (scripts[scriptName].getFields()[attributeName].type == "Entity" || scripts[scriptName].getFields()[attributeName].type == "UInt64")
                                fieldsValues[attributeName] = std::stoul(attribute->value());
                        }
                    }
                    this->_componentManager.addInstanceFields(scriptName, id, fieldsValues);
                }
                it++;
            }
        }
        this->_entityManager.updateMask(id, mask);
    }

    delete[] xmlCopy;
}

void Scene::setScene(EntityManager &entityManager, ComponentManager &componentManager)
{
    // can't use copy constructor because of mask category
    for (std::size_t i = 0; i < this->_entityManager.getMasks().size(); i++)
        if (this->_entityManager.getMasks()[i].has_value())
            entityManager.addMask(i, this->_entityManager.getMasks()[i].value());
    componentManager = this->_componentManager;
}
