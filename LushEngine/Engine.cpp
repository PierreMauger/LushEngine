#include "Engine.hpp"

using namespace Lush;

#define MODEL_TAG (ComponentType::TRANSFORM | ComponentType::MODEL)
#define BILLBOARD_TAG (ComponentType::TRANSFORM | ComponentType::BILLBOARD)
#define SKYBOX_TAG (ComponentType::CUBEMAP)
#define MAP_TAG (ComponentType::MAP)
#define CAMERA_TAG (ComponentType::TRANSFORM | ComponentType::CAMERA)
#define LIGHT_TAG (ComponentType::TRANSFORM | ComponentType::LIGHT)

Engine::Engine()
{
    this->_graphic = std::make_shared<Graphic>(1280, 720, "Lush Engine");
    this->_resourceManager = std::make_shared<ResourceManager>();
    this->_graphic->getRenderView().setShaders(this->_resourceManager->getShaders());

    this->_ecs.getEntityManager().addMaskCategory(MODEL_TAG);
    this->_ecs.getEntityManager().addMaskCategory(BILLBOARD_TAG);
    this->_ecs.getEntityManager().addMaskCategory(SKYBOX_TAG);
    this->_ecs.getEntityManager().addMaskCategory(MAP_TAG);
    this->_ecs.getEntityManager().addMaskCategory(CAMERA_TAG);
    this->_ecs.getEntityManager().addMaskCategory(LIGHT_TAG);

    for (std::size_t i = 0; i < this->_resourceManager->getScripts().size(); i++)
        this->_ecs.getEntityManager().addMaskCategory(ComponentType::COMPONENT_TYPE_COUNT << i);

    this->_ecs.getComponentManager().bindComponent<Transform>();
    this->_ecs.getComponentManager().bindComponent<Velocity>();
    this->_ecs.getComponentManager().bindComponent<Model>();
    this->_ecs.getComponentManager().bindComponent<Camera>();
    this->_ecs.getComponentManager().bindComponent<Light>();
    this->_ecs.getComponentManager().bindComponent<Cubemap>();
    this->_ecs.getComponentManager().bindComponent<Billboard>();
    this->_ecs.getComponentManager().bindComponent<Map>();

    this->_ecs.getSystemManager().bindSystem(std::make_unique<ScriptSystem>(this->_graphic, this->_resourceManager));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<CameraSystem>(this->_graphic));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<RenderSystem>(this->_graphic, this->_resourceManager));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<SceneSystem>(this->_graphic, this->_resourceManager));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<PickingSystem>(this->_graphic, this->_resourceManager));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<GUISystem>(this->_graphic, this->_resourceManager));
    this->_ecs.getSystemManager().bindSystem(std::make_unique<FileWatcherSystem>(this->_graphic, this->_resourceManager));

    this->_resourceManager->getScene()->setScene(this->_ecs.getEntityManager(), this->_ecs.getComponentManager());
}

void Engine::run()
{
    std::streambuf *oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(this->_graphic->getStringStream().rdbuf());

    while (!glfwWindowShouldClose(this->_graphic->getWindow())) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        this->updateMouse();

        this->_ecs.getSystemManager().updateSystems(this->_ecs.getEntityManager(), this->_ecs.getComponentManager(), this->_deltaTime);

        glfwSwapBuffers(this->_graphic->getWindow());
        glfwPollEvents();
        this->_deltaTime = glfwGetTime() - this->_lastTime;
        this->_lastTime = glfwGetTime();
    }
    std::cout.rdbuf(oldCoutStreamBuf);
}

void Engine::updateMouse()
{
    double x, y;
    glfwGetCursorPos(this->_graphic->getWindow(), &x, &y);
    if (this->_graphic->getMouseMovement() || this->_graphic->getSceneMovement())
        this->_graphic->setMouseOffset(glm::vec2(x, y));
    if (!this->_graphic->getMouseMovement())
        this->_graphic->setMousePosition(glm::vec2(x, y));
}
