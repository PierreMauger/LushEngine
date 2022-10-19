#include "Systems/Picking/PickingSystem.hpp"

using namespace Lush;

PickingSystem::PickingSystem(std::shared_ptr<Graphic> graphic)
{
    this->_graphic = graphic;

    glGenFramebuffers(1, &this->_buffer.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);
    glGenTextures(1, &this->_buffer.texture);
    glBindTexture(GL_TEXTURE_2D, this->_buffer.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 720, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_buffer.texture, 0);
    glGenRenderbuffers(1, &this->_buffer.depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->_buffer.depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->_buffer.depthbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PickingSystem::update(ComponentManager &componentManager, EntityManager &entityManager)
{
    auto &masks = entityManager.getMasks();
    std::size_t renderable = (ComponentType::TRANSFORM | ComponentType::MODELID);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->_graphic->getCamera().use("Picking");
    this->_graphic->getCamera().setView(glfwGetTime());

    for (std::size_t i = 0; i < masks.size(); i++) {
        if (masks[i].has_value() && (masks[i].value() & renderable) == renderable) {
            Transform transform = componentManager.getComponent<Transform>(i);
            ModelID modelID = componentManager.getComponent<ModelID>(i);

            glm::vec4 color;
            color.r = (((i + 1) & 0x000000FF) >> 0) / 255.0f;
            color.g = (((i + 1) & 0x0000FF00) >> 8) / 255.0f;
            color.b = (((i + 1) & 0x00FF0000) >> 16) / 255.0f;
            color.a = 1.0f;

            this->_graphic->getShaders()["Picking"].setVec4("id", color);
            this->_graphic->getCamera().setOnModel(transform);
            if (this->_graphic->getModels().find(modelID.id) != this->_graphic->getModels().end())
                this->_graphic->getModels()[modelID.id].draw(this->_graphic->getCamera().getShader());
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    double x = 0.0f;
    double y = 0.0f;
    glfwGetCursorPos(this->_graphic->getWindow().get(), &x, &y);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_buffer.framebuffer);
    unsigned char pixel[4];
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadPixels(x, 720 - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
    int hover = (pixel[0]) + (pixel[1] << 8) + (pixel[2] << 16);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::cout << hover << std::endl;
}
