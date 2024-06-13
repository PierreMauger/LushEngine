#include "Scene.hpp"

using namespace Lush;

static const char *cameraTypeNames[CameraType::CAMERA_TYPE_COUNT] = {"Perspective", "Orthographic"};
static const char *lightTypeNames[LightType::LIGHT_TYPE_COUNT] = {"Dir", "Point", "Spot", "Area"};
static const char *colliderTypeNames[ColliderType::COLLIDER_TYPE_COUNT] = {"Box", "Sphere", "Capsule", "Mesh"};
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
    {"UIElement", &Scene::loadUIElement},
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
    for (rapidxml::xml_node<> *entityNode = entitiesNode->first_node("Entity"); entityNode; entityNode = entityNode->next_sibling("Entity"))
        this->loadEntity(entityNode, scripts);
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
                Scene::loadScript(componentNode, entity, scripts[componentNode->name()]);
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
        for (rapidxml::xml_node<> *childNode = node->first_node("Children")->first_node("Entity"); childNode; childNode = childNode->next_sibling("Entity"))
            this->loadEntity(childNode, scripts, id);
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
    if (node->first_attribute("culling"))
        temp.culling = node->first_attribute("culling")->value() == std::string("false") ? false : true;
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

void Scene::loadUIElement(rapidxml::xml_node<> *node, Entity &entity)
{
    UIElement uiElement;
    if (node->first_attribute("size"))
        std::sscanf(node->first_attribute("size")->value(), "%f %f", &uiElement.size.x, &uiElement.size.y);
    if (node->first_attribute("offset"))
        std::sscanf(node->first_attribute("offset")->value(), "%f %f", &uiElement.offset.x, &uiElement.offset.y);
    if (node->first_attribute("anchor"))
        uiElement.anchor = std::stoi(node->first_attribute("anchor")->value());
    uiElement.textureName = node->first_attribute("textureName")->value();
    entity.addComponent(uiElement);
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
            else if (script.getFields()[attribute->name()].type == "Vector3") {
                glm::vec3 value;
                std::sscanf(attribute->value(), "%f %f %f", &value.x, &value.y, &value.z);
                scriptComponent.addField(attribute->name(), value);
            }
        }
    }
    entity.addScriptComponent(node->name(), scriptComponent);
}

void Scene::save(std::unordered_map<std::string, ScriptClass> &scripts)
{
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<> *rootNode = doc.allocate_node(rapidxml::node_element, "Scene");
    doc.append_node(rootNode);

    rapidxml::xml_node<> *entitiesNode = doc.allocate_node(rapidxml::node_element, "Entities");
    rootNode->append_node(entitiesNode);

    for (auto &[id, entity] : this->_entityManager->getEntities()) {
        if (entity.getParent().has_value())
            continue;
        Scene::saveEntity(entitiesNode, scripts, id, entity);
    }

    std::string xml;
    rapidxml::print(std::back_inserter(xml), doc);
    this->getFiles()[0].save(xml);
}

void Scene::saveEntity(rapidxml::xml_node<> *node, std::unordered_map<std::string, ScriptClass> &scripts, std::size_t entityId, Entity &entity)
{
    rapidxml::xml_document<> *doc = node->document();
    rapidxml::xml_node<> *entityNode = doc->allocate_node(rapidxml::node_element, "Entity");
    node->append_node(entityNode);

    entityNode->append_attribute(doc->allocate_attribute("id", doc->allocate_string(std::to_string(entityId).c_str())));
    entityNode->append_attribute(doc->allocate_attribute("name", entity.getName().c_str()));

    rapidxml::xml_node<> *componentsNode = doc->allocate_node(rapidxml::node_element, "Components");
    entityNode->append_node(componentsNode);

    if (entity.hasComponent<Transform>()) {
        rapidxml::xml_node<> *transformNode = doc->allocate_node(rapidxml::node_element, "Transform");
        componentsNode->append_node(transformNode);

        Scene::saveTransform(transformNode, entity);
    }
    if (entity.hasComponent<Model>()) {
        rapidxml::xml_node<> *modelNode = doc->allocate_node(rapidxml::node_element, "Model");
        componentsNode->append_node(modelNode);

        Scene::saveModel(modelNode, entity);
    }
    if (entity.hasComponent<Camera>()) {
        rapidxml::xml_node<> *cameraNode = doc->allocate_node(rapidxml::node_element, "Camera");
        componentsNode->append_node(cameraNode);

        Scene::saveCamera(cameraNode, entity);
    }
    if (entity.hasComponent<Light>()) {
        rapidxml::xml_node<> *lightNode = doc->allocate_node(rapidxml::node_element, "Light");
        componentsNode->append_node(lightNode);

        Scene::saveLight(lightNode, entity);
    }
    if (entity.hasComponent<Cubemap>()) {
        rapidxml::xml_node<> *cubemapNode = doc->allocate_node(rapidxml::node_element, "Cubemap");
        componentsNode->append_node(cubemapNode);
        Cubemap &cubemap = entity.getComponent<Cubemap>();

        cubemapNode->append_attribute(doc->allocate_attribute("name", cubemap.name.c_str()));
    }
    if (entity.hasComponent<Billboard>()) {
        rapidxml::xml_node<> *billboardNode = doc->allocate_node(rapidxml::node_element, "Billboard");
        componentsNode->append_node(billboardNode);

        Scene::saveBillboard(billboardNode, entity);
    }
    if (entity.hasComponent<Map>()) {
        rapidxml::xml_node<> *mapNode = doc->allocate_node(rapidxml::node_element, "Map");
        componentsNode->append_node(mapNode);

        Scene::saveMap(mapNode, entity);
    }
    if (entity.hasComponent<RigidBody>()) {
        rapidxml::xml_node<> *rigidBodyNode = doc->allocate_node(rapidxml::node_element, "RigidBody");
        componentsNode->append_node(rigidBodyNode);

        Scene::saveRigidBody(rigidBodyNode, entity);
    }
    if (entity.hasComponent<Collider>()) {
        rapidxml::xml_node<> *colliderNode = doc->allocate_node(rapidxml::node_element, "Collider");
        componentsNode->append_node(colliderNode);

        Scene::saveCollider(colliderNode, entity);
    }
    if (entity.hasComponent<CharacterController>()) {
        rapidxml::xml_node<> *characterControllerNode = doc->allocate_node(rapidxml::node_element, "CharacterController");
        componentsNode->append_node(characterControllerNode);

        Scene::saveCharacterController(characterControllerNode, entity);
    }
    if (entity.hasComponent<UIElement>()) {
        rapidxml::xml_node<> *uiElementNode = doc->allocate_node(rapidxml::node_element, "UIElement");
        componentsNode->append_node(uiElementNode);

        Scene::saveUIElement(uiElementNode, entity);
    }
    for (auto &[name, scriptClass] : scripts) {
        if (entity.hasScriptComponent(name)) {
            rapidxml::xml_node<> *scriptNode = doc->allocate_node(rapidxml::node_element, name.c_str());
            componentsNode->append_node(scriptNode);

            Scene::saveScript(scriptNode, entity, scriptClass);
        }
    }
    for (auto &childId : entity.getChildren()) {
        rapidxml::xml_node<> *childrenNode = doc->allocate_node(rapidxml::node_element, "Children");
        entityNode->append_node(childrenNode);

        this->saveEntity(childrenNode, scripts, childId, this->_entityManager->getEntity(childId));
    }
}

void Scene::saveTransform(rapidxml::xml_node<> *node, Entity &entity)
{
    Transform &transform = entity.getComponent<Transform>();
    if (transform.position != glm::vec3(0.0f)) {
        std::stringstream positionStream;
        positionStream << transform.position.x << " " << transform.position.y << " " << transform.position.z;
        char *positionValue = node->document()->allocate_string(positionStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("position", positionValue));
    }
    if (transform.rotation != glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) {
        glm::vec3 rotation = glm::round(glm::degrees(glm::eulerAngles(transform.rotation)) * 100.0f) / 100.0f;
        std::stringstream rotationStream;
        rotationStream << rotation.x << " " << rotation.y << " " << rotation.z;
        char *rotationValue = node->document()->allocate_string(rotationStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("rotation", rotationValue));
    }
    if (transform.scale != glm::vec3(1.0f)) {
        std::stringstream scaleStream;
        scaleStream << transform.scale.x << " " << transform.scale.y << " " << transform.scale.z;
        char *scaleValue = node->document()->allocate_string(scaleStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("scale", scaleValue));
    }
}

void Scene::saveModel(rapidxml::xml_node<> *node, Entity &entity)
{
    Model &model = entity.getComponent<Model>();
    if (model.name != "")
        node->append_attribute(node->document()->allocate_attribute("name", model.name.c_str()));
    if (!model.culling)
        node->append_attribute(node->document()->allocate_attribute("culling", "false"));
    for (auto &[name, material] : model.materials) {
        rapidxml::xml_node<> *materialNode = node->document()->allocate_node(rapidxml::node_element, "Material");
        node->append_node(materialNode);
        materialNode->append_attribute(node->document()->allocate_attribute("name", name.c_str()));

        std::stringstream diffuseStream;
        diffuseStream << material.diffuse.x << " " << material.diffuse.y << " " << material.diffuse.z;
        char *diffuseValue = node->document()->allocate_string(diffuseStream.str().c_str());
        materialNode->append_attribute(node->document()->allocate_attribute("diffuse", diffuseValue));
        if (material.ambient != glm::vec3(0.0f)) {
            std::stringstream ambientStream;
            ambientStream << material.ambient.x << " " << material.ambient.y << " " << material.ambient.z;
            char *ambientValue = node->document()->allocate_string(ambientStream.str().c_str());
            materialNode->append_attribute(node->document()->allocate_attribute("ambient", ambientValue));
        }
        if (material.specular != glm::vec3(0.0f)) {
            std::stringstream specularStream;
            specularStream << material.specular.x << " " << material.specular.y << " " << material.specular.z;
            char *specularValue = node->document()->allocate_string(specularStream.str().c_str());
            materialNode->append_attribute(node->document()->allocate_attribute("specular", specularValue));
        }
        if (material.emission != glm::vec3(0.0f)) {
            std::stringstream emissionStream;
            emissionStream << material.emission.x << " " << material.emission.y << " " << material.emission.z;
            char *emissionValue = node->document()->allocate_string(emissionStream.str().c_str());
            materialNode->append_attribute(node->document()->allocate_attribute("emission", emissionValue));
        }
    }
}

void Scene::saveCamera(rapidxml::xml_node<> *node, Entity &entity)
{
    Camera &camera = entity.getComponent<Camera>();
    if (camera.type != CameraType::PERSPECTIVE)
        node->append_attribute(node->document()->allocate_attribute("type", cameraTypeNames[camera.type]));
    if (camera.forward != glm::vec3(0.0f, 0.0f, -1.0f)) {
        std::stringstream forwardStream;
        forwardStream << camera.forward.x << " " << camera.forward.y << " " << camera.forward.z;
        char *forwardValue = node->document()->allocate_string(forwardStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("forward", forwardValue));
    }
    if (camera.fov != 45.0f) {
        std::stringstream fovStream;
        fovStream << camera.fov;
        char *fovValue = node->document()->allocate_string(fovStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("fov", fovValue));
    }
    if (camera.near != 0.1f) {
        std::stringstream nearStream;
        nearStream << camera.near;
        char *nearValue = node->document()->allocate_string(nearStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("near", nearValue));
    }
    if (camera.far != 100.0f) {
        std::stringstream farStream;
        farStream << camera.far;
        char *farValue = node->document()->allocate_string(farStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("far", farValue));
    }
}

void Scene::saveLight(rapidxml::xml_node<> *node, Entity &entity)
{
    Light &light = entity.getComponent<Light>();
    if (light.type != LightType::DIRECTIONAL)
        node->append_attribute(node->document()->allocate_attribute("type", lightTypeNames[light.type]));
    if (light.shadowSize != glm::vec3(20.0f)) {
        std::stringstream shadowSizeStream;
        shadowSizeStream << light.shadowSize.x << " " << light.shadowSize.y << " " << light.shadowSize.z;
        char *shadowSizeValue = node->document()->allocate_string(shadowSizeStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("shadowSize", shadowSizeValue));
    }
    if (light.intensity != 1.0f) {
        std::stringstream intensityStream;
        intensityStream << light.intensity;
        char *intensityValue = node->document()->allocate_string(intensityStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("intensity", intensityValue));
    }
    if (light.color != glm::vec3(1.0f)) {
        std::stringstream colorStream;
        colorStream << light.color.x << " " << light.color.y << " " << light.color.z;
        char *colorValue = node->document()->allocate_string(colorStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("color", colorValue));
    }
    if (light.cutOff != 0.0f) {
        std::stringstream cutOffStream;
        cutOffStream << light.cutOff;
        char *cutOffValue = node->document()->allocate_string(cutOffStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("cutOff", cutOffValue));
    }
}

void Scene::saveBillboard(rapidxml::xml_node<> *node, Entity &entity)
{
    Billboard &billboard = entity.getComponent<Billboard>();
    node->append_attribute(node->document()->allocate_attribute("name", billboard.name.c_str()));
    if (billboard.lockYAxis)
        node->append_attribute(node->document()->allocate_attribute("lockYAxis", "true"));
}

void Scene::saveMap(rapidxml::xml_node<> *node, Entity &entity)
{
    Map &map = entity.getComponent<Map>();
    node->append_attribute(node->document()->allocate_attribute("heightMap", map.heightMap.c_str()));
    node->append_attribute(node->document()->allocate_attribute("diffuseTexture", map.diffuseTexture.c_str()));
    node->append_attribute(node->document()->allocate_attribute("normalTexture", map.normalTexture.c_str()));
    node->append_attribute(node->document()->allocate_attribute("diffuseTexture2", map.diffuseTexture2.c_str()));
    node->append_attribute(node->document()->allocate_attribute("diffuseTexture3", map.diffuseTexture3.c_str()));
}

void Scene::saveRigidBody(rapidxml::xml_node<> *node, Entity &entity)
{
    RigidBody &rigidBody = entity.getComponent<RigidBody>();
    if (rigidBody.mass != 1.0f) {
        std::stringstream massStream;
        massStream << rigidBody.mass;
        char *massValue = node->document()->allocate_string(massStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("mass", massValue));
    }
    if (rigidBody.friction != 0.5f) {
        std::stringstream frictionStream;
        frictionStream << rigidBody.friction;
        char *frictionValue = node->document()->allocate_string(frictionStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("friction", frictionValue));
    }
    if (rigidBody.restitution != 0.0f) {
        std::stringstream restitutionStream;
        restitutionStream << rigidBody.restitution;
        char *restitutionValue = node->document()->allocate_string(restitutionStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("restitution", restitutionValue));
    }
    if (rigidBody.kinematic)
        node->append_attribute(node->document()->allocate_attribute("kinematic", "true"));
}

void Scene::saveCollider(rapidxml::xml_node<> *node, Entity &entity)
{
    Collider &collider = entity.getComponent<Collider>();
    if (collider.type != ColliderType::BOX)
        node->append_attribute(node->document()->allocate_attribute("type", colliderTypeNames[collider.type]));
    if (collider.center != glm::vec3(0.0f)) {
        std::stringstream centerStream;
        centerStream << collider.center.x << " " << collider.center.y << " " << collider.center.z;
        char *centerValue = node->document()->allocate_string(centerStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("center", centerValue));
    }
    if (collider.type == ColliderType::SPHERE) {
        if (collider.size.x != 0.0f) {
            std::stringstream radiusStream;
            radiusStream << collider.size.x;
            char *radiusValue = node->document()->allocate_string(radiusStream.str().c_str());
            node->append_attribute(node->document()->allocate_attribute("radius", radiusValue));
        }
    } else if (collider.type == ColliderType::CAPSULE) {
        if (collider.size.x != 0.0f && collider.size.y != 0.0f) {
            std::stringstream sizeStream;
            sizeStream << collider.size.x << " " << collider.size.y;
            char *sizeValue = node->document()->allocate_string(sizeStream.str().c_str());
            node->append_attribute(node->document()->allocate_attribute("radius", sizeValue));
        }
    } else {
        if (collider.size != glm::vec3(0.0f)) {
            std::stringstream sizeStream;
            sizeStream << collider.size.x << " " << collider.size.y << " " << collider.size.z;
            char *sizeValue = node->document()->allocate_string(sizeStream.str().c_str());
            node->append_attribute(node->document()->allocate_attribute("size", sizeValue));
        }
    }
    if (collider.tag != "")
        node->append_attribute(node->document()->allocate_attribute("tag", collider.tag.c_str()));
}

void Scene::saveCharacterController(rapidxml::xml_node<> *node, Entity &entity)
{
    CharacterController &characterController = entity.getComponent<CharacterController>();
    if (characterController.center != glm::vec3(0.0f)) {
        std::stringstream centerStream;
        centerStream << characterController.center.x << " " << characterController.center.y << " " << characterController.center.z;
        char *centerValue = node->document()->allocate_string(centerStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("center", centerValue));
    }
    if (characterController.slopeLimit != 45.0f) {
        std::stringstream slopeLimitStream;
        slopeLimitStream << characterController.slopeLimit;
        char *slopeLimitValue = node->document()->allocate_string(slopeLimitStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("slopeLimit", slopeLimitValue));
    }
    if (characterController.stepOffset != 1.0f) {
        std::stringstream stepOffsetStream;
        stepOffsetStream << characterController.stepOffset;
        char *stepOffsetValue = node->document()->allocate_string(stepOffsetStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("stepOffset", stepOffsetValue));
    }
}

void Scene::saveUIElement(rapidxml::xml_node<> *node, Entity &entity)
{
    UIElement &uiElement = entity.getComponent<UIElement>();
    if (uiElement.size != glm::vec2(100.0f)) {
        std::stringstream sizeStream;
        sizeStream << uiElement.size.x << " " << uiElement.size.y;
        char *sizeValue = node->document()->allocate_string(sizeStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("size", sizeValue));
    }
    if (uiElement.offset != glm::vec2(0.0f)) {
        std::stringstream offsetStream;
        offsetStream << uiElement.offset.x << " " << uiElement.offset.y;
        char *offsetValue = node->document()->allocate_string(offsetStream.str().c_str());
        node->append_attribute(node->document()->allocate_attribute("offset", offsetValue));
    }
    if (uiElement.anchor != 0)
        node->append_attribute(node->document()->allocate_attribute("anchor", node->document()->allocate_string(std::to_string(uiElement.anchor).c_str())));
    if (uiElement.textureName != "")
        node->append_attribute(node->document()->allocate_attribute("textureName", uiElement.textureName.c_str()));
}

void Scene::saveScript(rapidxml::xml_node<> *node, Entity &entity, ScriptClass &script)
{
    ScriptComponent &scriptComponent = entity.getScriptComponent(node->name());
    for (auto &[name, field] : script.getFields()) {
        if (scriptComponent.hasField(name)) {
            rapidxml::xml_document<> *doc = node->document();
            if (field.type == "Single") {
                if (scriptComponent.getField<float>(name) == 0.0f)
                    continue;
                std::stringstream fieldStream;
                fieldStream << scriptComponent.getField<float>(name);
                char *fieldValue = doc->allocate_string(fieldStream.str().c_str());
                node->append_attribute(doc->allocate_attribute(name.c_str(), fieldValue));
            } else if (field.type == "Entity" || field.type == "UInt64") {
                if (scriptComponent.getField<std::size_t>(name) == 0)
                    continue;
                std::stringstream fieldStream;
                fieldStream << scriptComponent.getField<std::size_t>(name);
                char *fieldValue = doc->allocate_string(fieldStream.str().c_str());
                node->append_attribute(doc->allocate_attribute(name.c_str(), fieldValue));
            } else if (field.type == "String") {
                if (scriptComponent.getField<std::string>(name) == "")
                    continue;
                char *fieldValue = doc->allocate_string(scriptComponent.getField<std::string>(name).c_str());
                node->append_attribute(doc->allocate_attribute(name.c_str(), fieldValue));
            }
        }
    }
}
