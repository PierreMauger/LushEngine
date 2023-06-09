#include "Scene.hpp"

using namespace Lush;

Scene::Scene(File &file, std::unordered_map<std::string, ScriptClass> &scripts) : Resource(ResourceType::SCENE, file)
{
    this->load(file, scripts);
}

EntityManager &Scene::getEntityManager()
{
    return this->_entityManager;
}

void Scene::load(File &file, std::unordered_map<std::string, ScriptClass> &scripts)
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
        Entity entity;

        rapidxml::xml_node<> *componentsNode = entityNode->first_node("Components");
        for (rapidxml::xml_node<> *componentNode = componentsNode->first_node(); componentNode; componentNode = componentNode->next_sibling()) {
            std::string name = componentNode->name();
            if (name == "Transform") {
                Transform temp;
                std::sscanf(componentNode->first_attribute("position")->value(), "%f %f %f", &temp.position.x, &temp.position.y, &temp.position.z);
                std::sscanf(componentNode->first_attribute("rotation")->value(), "%f %f %f", &temp.rotation.x, &temp.rotation.y, &temp.rotation.z);
                std::sscanf(componentNode->first_attribute("scale")->value(), "%f %f %f", &temp.scale.x, &temp.scale.y, &temp.scale.z);
                entity.addComponent(temp);
            } else if (name == "Model") {
                Model temp;
                temp.name = componentNode->first_attribute("name")->value();
                entity.addComponent(temp);
            } else if (name == "Camera") {
                Camera temp;
                // attributes
                entity.addComponent(temp);
            } else if (name == "Light") {
                Light temp;
                if (componentNode->first_attribute("type"))
                    temp.type = (LightType)std::atoi(componentNode->first_attribute("type")->value());
                // attributes
                entity.addComponent(temp);
            } else if (name == "Cubemap") {
                Cubemap temp;
                temp.name = componentNode->first_attribute("name")->value();
                entity.addComponent(temp);
            } else if (name == "Billboard") {
                Billboard temp;
                temp.name = componentNode->first_attribute("name")->value();
                entity.addComponent(temp);
            } else if (name == "Map") {
                Map temp;
                temp.heightMap = componentNode->first_attribute("heightMap")->value();
                temp.diffuseTexture = componentNode->first_attribute("diffuseTexture")->value();
                temp.normalTexture = componentNode->first_attribute("normalTexture")->value();
                temp.diffuseTexture2 = componentNode->first_attribute("diffuseTexture2")->value();
                temp.diffuseTexture3 = componentNode->first_attribute("diffuseTexture3")->value();
                entity.addComponent(temp);
            }
            for (auto &[scriptName, script] : scripts) {
                if (scriptName == name) {
                    ScriptComponent scriptComponent;
                    for (auto &[fieldName, field] : script.getFields()) {
                        if (field.type == "Single")
                            scriptComponent.addField(fieldName, 0.0f);
                        if (field.type == "Entity" || field.type == "UInt64")
                            scriptComponent.addField(fieldName, (unsigned long)0);
                    }
                    for (rapidxml::xml_attribute<> *attribute = componentNode->first_attribute(); attribute; attribute = attribute->next_attribute()) {
                        std::string attributeName = attribute->name();
                        if (scripts[scriptName].getFields().find(attributeName) != scripts[scriptName].getFields().end()) {
                            if (scripts[scriptName].getFields()[attributeName].type == "Single")
                                scriptComponent.addField(attributeName, std::stof(attribute->value()));
                            if (scripts[scriptName].getFields()[attributeName].type == "Entity" || scripts[scriptName].getFields()[attributeName].type == "UInt64")
                                scriptComponent.addField(attributeName, std::stoul(attribute->value()));
                        }
                    }
                    entity.addScriptComponent(scriptName, scriptComponent);
                }
            }
        }
        this->_entityManager.addEntity(entity, id);
    }
    delete[] xmlCopy;
}

void Scene::reload(File &file, std::unordered_map<std::string, ScriptClass> &scripts)
{
    this->load(file, scripts);
}

void Scene::setScene(EntityManager &entityManager)
{
    entityManager.clear();
    for (auto &[id, entity] : this->_entityManager.getEntities())
        entityManager.addEntity(entity, id);
}
