#include "RenderObject.hpp"

using namespace Lush;

RenderObject::RenderObject(GameObject obj)
{
    this->_type = obj.getType();
    this->_name = obj.getName();
    this->_position = obj.getPosition();
    this->_scale = obj.getScale();
    this->_rotation = obj.getRotation();
    this->_isHovered = false;
    this->_isSelected = false;
    this->_polygonMode = false;
    this->_lightSource = false;
}

glm::vec3 RenderObject::getPosition() const
{
    return this->_position;
}

glm::vec3 RenderObject::getScale() const
{
    return this->_scale;
}

glm::vec3 RenderObject::getRotation() const
{
    return this->_rotation;
}

bool RenderObject::isSelected() const
{
    return this->_isSelected;
}

bool RenderObject::isLightSource() const
{
    return this->_lightSource;
}

void RenderObject::setPosition(glm::vec3 position)
{
    this->_position = position;
}

void RenderObject::setScale(glm::vec3 scale)
{
    this->_scale = scale;
}

void RenderObject::setRotation(glm::vec3 rotation)
{
    this->_rotation = rotation;
}

void RenderObject::setHovered(bool isHovered)
{
    this->_isHovered = isHovered;
}

void RenderObject::setSelected(bool isSelected)
{
    this->_isSelected = isSelected;
}
