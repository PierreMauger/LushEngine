#include "GameObject.hpp"

using namespace Lush;

GameObject::GameObject(int type, std::string name, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    this->_type = type;
    this->_name = name;
    this->_position = position;
    this->_scale = scale;
    this->_rotation = rotation;
}

int GameObject::getType() const
{
    return this->_type;
}

std::string GameObject::getName() const
{
    return this->_name;
}

glm::vec3 GameObject::getPos() const
{
    return this->_position;
}

glm::vec3 GameObject::getScale() const
{
    return this->_scale;
}

glm::vec3 GameObject::getRotation() const
{
    return this->_rotation;
}

void GameObject::setType(int type)
{
    this->_type = type;
}

void GameObject::setName(std::string name)
{
    this->_name = name;
}

void GameObject::setPos(glm::vec3 pos)
{
    this->_position = pos;
}

void GameObject::setScale(glm::vec3 scale)
{
    this->_scale = scale;
}

void GameObject::setRotation(glm::vec3 rotation)
{
    this->_rotation = rotation;
}

void GameObject::move(glm::vec3 speed)
{
    this->_position += speed;
}
