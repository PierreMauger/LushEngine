#include "Scene.hpp"

using namespace Lush;

static const char *cameraTypeNames[CameraType::CAMERA_TYPE_COUNT] = {"Perspective", "Orthographic"};
static const char *lightTypeNames[LightType::LIGHT_TYPE_COUNT] = {"Dir", "Point", "Spot", "Area"};
static const char *colliderTypeNames[ColliderType::COLLIDER_TYPE_COUNT] = {"box", "Sphere", "Capsule", "Mesh"};
static const std::unordered_map<std::string, std::function<void(rapidxml::xml_node<> *, Entity &)>> componentLoaders = {
    {"Transform", &Scene::loadTransform},
    {"Model", &Scene::loadModel},
    {"Camera", &Scene::loadCamera},
    {"Light", &Scene::loadLight},
    {"Cubemap", &Scene::loadCubemap},
    {"Billboard", &Scene::loadBillboard},
    {"Map", &Scene::loadMap},
    {"RigidBody", &Scene::loadRigidBody},
    {"Collider", &Scene::loadCollider},
    {"CharacterController", &Scene::loadCharacterController},
};

Scene::Scene(File &file, std::unordered_map<std::string, ScriptClass> &scripts) : Resource(ResourceType::SCENE, file)
{
    this->_entityManager = std::make_shared<EntityManager>();
    this->load(file, scripts);
}

std::shared_ptr<EntityManager> Scene::getEntityManager()
{
    return this->_entityManager;
}

void Scene::load(const File &file, std::unordered_map<std::string, ScriptClass> &scripts)
{
    rapidxml::xml_document<> doc;
    std::string xml = file.load();
    doc.parse<0>((char *)xml.c_str());

    rapidxml::xml_node<> *rootNode = doc.first_node("Scene");
    rapidxml::xml_node<> *entitiesNode = rootNode->first_node("Entities");
    for (rapidxml::xml_node<> *entityNode = entitiesNode->first_node("Entity"); entityNode; entityNode = entityNode->next_sibling("Entity")) {
        this->loadEntity(entityNode, scripts);
    }
}

void Scene::reload(const File &file, std::unordered_map<std::string, ScriptClass> &scripts)
{
    this->_entityManager->clear();
    this->load(file, scripts);
}

void Scene::loadEntity(rapidxml::xml_node<> *node, std::unordered_map<std::string, ScriptClass> &scripts, std::size_t parentId)
{
    int id = 0;
    if (node->first_attribute("id"))
        id = std::atoi(node->first_attribute("id")->value());
    else if (!this->_entityManager->getEntities().empty())
        id = this->_entityManager->getEntities().rbegin()->first + 1;

    Entity entity;
    if (node->first_attribute("name"))
        entity.setName(node->first_attribute("name")->value());

    if (node->first_node("Components")) {
        for (rapidxml::xml_node<> *componentNode = node->first_node("Components")->first_node(); componentNode; componentNode = componentNode->next_sibling()) {
            if (componentLoaders.contains(componentNode->name()))
                componentLoaders.at(componentNode->name())(componentNode, entity);
            else if (scripts.contains(componentNode->name()))
                Lush::Scene::loadScript(componentNode, entity, scripts[componentNode->name()]);
            else
                std::cout << "Component loader not found for " << componentNode->name() << std::endl;
        }
    }
    if (parentId != -1) {
        entity.setParent(parentId);
        this->_entityManager->getEntity(parentId).addChild(id);
    }
    this->_entityManager->addEntity(entity, id);
    if (node->first_node("Children")) {
        for (rapidxml::xml_node<> *childNode = node->first_node("Children")->first_node("Entity"); childNode; childNode = childNode->next_sibling("Entity")) {
            this->loadEntity(childNode, scripts, id);
        }
    }
}

void Scene::loadTransform(rapidxml::xml_node<> *node, Entity &entity)
{
    Transform transform;
    if (node->first_attribute("position"))
        std::sscanf(node->first_attribute("position")->value(), "%f %f %f", &transform.position.x, &transform.position.y, &transform.position.z);
    if (node->first_attribute("rotation")) {
        glm::vec3 rotation;
        std::sscanf(node->first_attribute("rotation")->value(), "%f %f %f", &rotation.x, &rotation.y, &rotation.z);
        transform.rotation = glm::quat(glm::radians(rotation));
    }
    if (node->first_attribute("scale"))
        std::sscanf(node->first_attribute("scale")->value(), "%f %f %f", &transform.scale.x, &transform.scale.y, &transform.scale.z);
    entity.addComponent(transform);
}

void Scene::loadModel(rapidxml::xml_node<> *node, Entity &entity)
{
    Model temp;
    if (node->first_attribute("name"))
        temp.name = node->first_attribute("name")->value();
    for (rapidxml::xml_node<> *materialNode = node->first_node("Material"); materialNode; materialNode = materialNode->next_sibling("Material")) {
        Material material;
        std::string name;
        if (materialNode->first_attribute("name"))
            name = materialNode->first_attribute("name")->value();
        else
            name = "default";
        if (materialNode->first_attribute("diffuse"))
            std::sscanf(materialNode->first_attribute("diffuse")->value(), "%f %f %f", &material.diffuse.x, &material.diffuse.y, &material.diffuse.z);
        if (materialNode->first_attribute("ambient"))
            std::sscanf(materialNode->first_attribute("ambient")->value(), "%f %f %f", &material.ambient.x, &material.ambient.y, &material.ambient.z);
        if (materialNode->first_attribute("specular"))
            std::sscanf(materialNode->first_attribute("specular")->value(), "%f %f %f", &material.specular.x, &material.specular.y, &material.specular.z);
        if (materialNode->first_attribute("emission"))
            std::sscanf(materialNode->first_attribute("emission")->value(), "%f %f %f", &material.emission.x, &material.emission.y, &material.emission.z);
        temp.materials[name] = material;
    }
    entity.addComponent(temp);
}

void Scene::loadCamera(rapidxml::xml_node<> *node, Entity &entity)
{
    Camera camera;
    if (node->first_attribute("type")) {
        auto it = std::find_if(std::begin(cameraTypeNames), std::end(cameraTypeNames), [&node](const std::string &name) { return name == node->first_attribute("type")->value(); });
        if (it != std::end(cameraTypeNames))
            camera.type = (CameraType)(it - std::begin(cameraTypeNames));
    }
    if (node->first_attribute("forward"))
        std::sscanf(node->first_attribute("forward")->value(), "%f %f %f", &camera.forward.x, &camera.forward.y, &camera.forward.z);
    if (node->first_attribute("fov"))
        camera.fov = std::atof(node->first_attribute("fov")->value());
    if (node->first_attribute("near"))
        camera.near = std::atof(node->first_attribute("near")->value());
    if (node->first_attribute("far"))
        camera.far = std::atof(node->first_attribute("far")->value());
    entity.addComponent(camera);
}

void Scene::loadLight(rapidxml::xml_node<> *node, Entity &entity)
{
    Light light;
    if (node->first_attribute("type")) {
        auto it = std::find_if(std::begin(lightTypeNames), std::end(lightTypeNames), [&node](const std::string &name) { return name == node->first_attribute("type")->value(); });
        if (it != std::end(lightTypeNames))
            light.type = (LightType)(it - std::begin(lightTypeNames));
    }
    if (node->first_attribute("color"))
        std::sscanf(node->first_attribute("color")->value(), "%f %f %f", &light.color.x, &light.color.y, &light.color.z);
    if (node->first_attribute("intensity"))
        light.intensity = std::atof(node->first_attribute("intensity")->value());
    if (node->first_attribute("cutOff"))
        light.cutOff = std::atof(node->first_attribute("cutOff")->value());
    entity.addComponent(light);
}

void Scene::loadCubemap(rapidxml::xml_node<> *node, Entity &entity)
{
    Cubemap cubemap;
    cubemap.name = node->first_attribute("name")->value();
    entity.addComponent(cubemap);
}

void Scene::loadBillboard(rapidxml::xml_node<> *node, Entity &entity)
{
    Billboard billboard;
    billboard.name = node->first_attribute("name")->value();
    if (node->first_attribute("lockYAxis"))
        billboard.lockYAxis = node->first_attribute("lockYAxis")->value() == std::string("true") ? true : false;
    entity.addComponent(billboard);
}

void Scene::loadMap(rapidxml::xml_node<> *node, Entity &entity)
{
    Map map;
    map.heightMap = node->first_attribute("heightMap")->value();
    map.diffuseTexture = node->first_attribute("diffuseTexture")->value();
    map.normalTexture = node->first_attribute("normalTexture")->value();
    map.diffuseTexture2 = node->first_attribute("diffuseTexture2")->value();
    map.diffuseTexture3 = node->first_attribute("diffuseTexture3")->value();
    entity.addComponent(map);
}

void Scene::loadRigidBody(rapidxml::xml_node<> *node, Entity &entity)
{
    RigidBody rigidBody;
    if (node->first_attribute("mass"))
        rigidBody.mass = std::stof(node->first_attribute("mass")->value());
    if (node->first_attribute("friction"))
        rigidBody.friction = std::stof(node->first_attribute("friction")->value());
    if (node->first_attribute("restitution"))
        rigidBody.restitution = std::stof(node->first_attribute("restitution")->value());
    if (node->first_attribute("kinematic"))
        rigidBody.kinematic = std::atoi(node->first_attribute("kinematic")->value());
    entity.addComponent(rigidBody);
}

void Scene::loadCollider(rapidxml::xml_node<> *node, Entity &entity)
{
    Collider collider;
    if (node->first_attribute("type")) {
        auto it =
            std::find_if(std::begin(colliderTypeNames), std::end(colliderTypeNames), [&node](const std::string &name) { return name == node->first_attribute("type")->value(); });
        if (it != std::end(colliderTypeNames))
            collider.type = (ColliderType)(it - std::begin(colliderTypeNames));
    }
    if (node->first_attribute("center"))
        std::sscanf(node->first_attribute("center")->value(), "%f %f %f", &collider.center.x, &collider.center.y, &collider.center.z);
    if (collider.type == ColliderType::SPHERE) {
        if (node->first_attribute("radius"))
            std::sscanf(node->first_attribute("radius")->value(), "%f", &collider.size.x);
    } else if (collider.type == ColliderType::CAPSULE) {
        if (node->first_attribute("radius"))
            std::sscanf(node->first_attribute("radius")->value(), "%f %f", &collider.size.x, &collider.size.y);
    } else {
        if (node->first_attribute("size"))
            std::sscanf(node->first_attribute("size")->value(), "%f %f %f", &collider.size.x, &collider.size.y, &collider.size.z);
    }
    if (node->first_attribute("tag"))
        collider.tag = node->first_attribute("tag")->value();
    entity.addComponent(collider);
}

void Scene::loadCharacterController(rapidxml::xml_node<> *node, Entity &entity)
{
    CharacterController characterController;
    if (node->first_attribute("center"))
        std::sscanf(node->first_attribute("center")->value(), "%f %f %f", &characterController.center.x, &characterController.center.y, &characterController.center.z);
    if (node->first_attribute("slopeLimit"))
        characterController.slopeLimit = std::stof(node->first_attribute("slopeLimit")->value());
    if (node->first_attribute("stepOffset"))
        characterController.stepOffset = std::stof(node->first_attribute("stepOffset")->value());
    entity.addComponent(characterController);
}

void Scene::loadScript(rapidxml::xml_node<> *node, Entity &entity, ScriptClass &script)
{
    ScriptComponent scriptComponent(script);

    for (rapidxml::xml_attribute<> *attribute = node->first_attribute(); attribute; attribute = attribute->next_attribute()) {
        if (script.getFields().contains(attribute->name())) {
            if (script.getFields()[attribute->name()].type == "Single")
                scriptComponent.addField(attribute->name(), std::stof(attribute->value()));
            else if (script.getFields()[attribute->name()].type == "Entity" || script.getFields()[attribute->name()].type == "UInt64")
                scriptComponent.addField(attribute->name(), std::stoul(attribute->value()));
            else if (script.getFields()[attribute->name()].type == "String")
                scriptComponent.addField(attribute->name(), std::string(attribute->value()));
        }
    }
    entity.addScriptComponent(node->name(), scriptComponent);
}
