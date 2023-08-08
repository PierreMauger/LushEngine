#include "Scene.hpp"

using namespace Lush;

static const char *lightTypeNames[LightType::LIGHT_TYPE_COUNT] = {"Dir", "Point", "Spot", "Area"};
static const char *colliderTypeNames[ColliderType::COLLIDER_TYPE_COUNT] = {"Box", "Sphere", "Capsule", "Mesh"};

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
        int id = 0;
        if (entityNode->first_attribute("id"))
            id = std::atoi(entityNode->first_attribute("id")->value());
        else
            id = this->_entityManager.getEntities().size();
        Entity entity;
        if (entityNode->first_attribute("name"))
            entity.setName(entityNode->first_attribute("name")->value());

        rapidxml::xml_node<> *componentsNode = entityNode->first_node("Components");
        for (rapidxml::xml_node<> *componentNode = componentsNode->first_node(); componentNode; componentNode = componentNode->next_sibling()) {
            std::string name = componentNode->name();
            if (name == "Transform") {
                Transform temp;
                if (componentNode->first_attribute("position"))
                    std::sscanf(componentNode->first_attribute("position")->value(), "%f %f %f", &temp.position.x, &temp.position.y, &temp.position.z);
                if (componentNode->first_attribute("rotation"))
                    std::sscanf(componentNode->first_attribute("rotation")->value(), "%f %f %f", &temp.rotation.x, &temp.rotation.y, &temp.rotation.z);
                if (componentNode->first_attribute("scale"))
                    std::sscanf(componentNode->first_attribute("scale")->value(), "%f %f %f", &temp.scale.x, &temp.scale.y, &temp.scale.z);
                entity.addComponent(temp);
            } else if (name == "Model") {
                Model temp;
                temp.name = componentNode->first_attribute("name")->value();
                entity.addComponent(temp);
            } else if (name == "Camera") {
                Camera temp;
                if (componentNode->first_attribute("forward"))
                    std::sscanf(componentNode->first_attribute("forward")->value(), "%f %f %f", &temp.forward.x, &temp.forward.y, &temp.forward.z);
                if (componentNode->first_attribute("fov"))
                    temp.fov = std::atof(componentNode->first_attribute("fov")->value());
                if (componentNode->first_attribute("near"))
                    temp.near = std::atof(componentNode->first_attribute("near")->value());
                if (componentNode->first_attribute("far"))
                    temp.far = std::atof(componentNode->first_attribute("far")->value());
                entity.addComponent(temp);
            } else if (name == "Light") {
                Light temp;
                if (componentNode->first_attribute("type")) {
                    for (int i = 0; i < LightType::LIGHT_TYPE_COUNT; i++) {
                        if (strcmp(lightTypeNames[i], componentNode->first_attribute("type")->value()) == 0) {
                            temp.type = (LightType)i;
                            break;
                        }
                    }
                }
                if (componentNode->first_attribute("color"))
                    std::sscanf(componentNode->first_attribute("color")->value(), "%f %f %f", &temp.color.x, &temp.color.y, &temp.color.z);
                if (componentNode->first_attribute("intensity"))
                    temp.intensity = std::atof(componentNode->first_attribute("intensity")->value());
                if (componentNode->first_attribute("cutOff"))
                    temp.cutOff = std::atof(componentNode->first_attribute("cutOff")->value());
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
            } else if (name == "RigidBody") {
                RigidBody temp;
                if (componentNode->first_attribute("mass"))
                    temp.mass = std::stof(componentNode->first_attribute("mass")->value());
                if (componentNode->first_attribute("friction"))
                    temp.friction = std::stof(componentNode->first_attribute("friction")->value());
                if (componentNode->first_attribute("restitution"))
                    temp.restitution = std::stof(componentNode->first_attribute("restitution")->value());
                if (componentNode->first_attribute("kinematic"))
                    temp.kinematic = std::atoi(componentNode->first_attribute("kinematic")->value());
                entity.addComponent(temp);
            } else if (name == "Collider") {
                Collider temp;
                if (componentNode->first_attribute("type")) {
                    for (int i = 0; i < ColliderType::COLLIDER_TYPE_COUNT; i++) {
                        if (strcmp(colliderTypeNames[i], componentNode->first_attribute("type")->value()) == 0) {
                            temp.type = (ColliderType)i;
                            break;
                        }
                    }
                }
                if (componentNode->first_attribute("center"))
                    std::sscanf(componentNode->first_attribute("center")->value(), "%f %f %f", &temp.center.x, &temp.center.y, &temp.center.z);
                if (componentNode->first_attribute("size"))
                    std::sscanf(componentNode->first_attribute("size")->value(), "%f %f %f", &temp.size.x, &temp.size.y, &temp.size.z);
                if (componentNode->first_attribute("tag"))
                    temp.tag = componentNode->first_attribute("tag")->value();
                entity.addComponent(temp);
            } else if (name == "CharacterController") {
                CharacterController temp;
                if (componentNode->first_attribute("center"))
                    std::sscanf(componentNode->first_attribute("center")->value(), "%f %f %f", &temp.center.x, &temp.center.y, &temp.center.z);
                if (componentNode->first_attribute("slopeLimit"))
                    temp.slopeLimit = std::stof(componentNode->first_attribute("slopeLimit")->value());
                if (componentNode->first_attribute("stepOffset"))
                    temp.stepOffset = std::stof(componentNode->first_attribute("stepOffset")->value());
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
