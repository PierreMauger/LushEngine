#include "Systems/Render/RenderSystem.hpp"

using namespace Lush;

RenderSystem::RenderSystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
    auto &masks = entityManager.getMasks();
    std::size_t renderable = (ComponentType::TRANSFORM | ComponentType::MODELID);

    this->_graphic->getCamera().use("Camera");
    for (std::size_t i = 0; i < masks.size(); i++) {
        if (masks[i].has_value() && (masks[i].value() & renderable) == renderable) {
            Transform transform = componentManager.getComponent<Transform>(i);
            Model model = componentManager.getComponent<Model>(i);

            this->_graphic->getCamera().setOnModel(transform);
            if (this->_graphic->getModels().find(model.id) != this->_graphic->getModels().end())
                this->_graphic->getModels()[model.id].draw(this->_graphic->getCamera().getShader());
        }
    }
}
