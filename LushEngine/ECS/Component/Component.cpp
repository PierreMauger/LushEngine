#include "ECS/Component/Component.hpp"

using namespace Lush;

Component::Component()
{
}

std::size_t Component::getSize()
{
    return this->_values.size();
}

std::optional<std::any> &Component::getValues(std::size_t id)
{
    if (id > this->_values.size())
        throw std::runtime_error("incorrect ID");
    return this->_values[id];
}

void Component::addValue(std::size_t id, std::any value)
{
    if (id >= this->_values.size())
        this->_values.resize(id + 1);
    this->_values[id] = value;
}

void Component::removeValue(std::size_t id)
{
    if (id >= this->_values.size())
        return;
    this->_values[id] = std::nullopt;
}

void Component::updateValue(std::size_t id, std::any value)
{
    if (id >= this->_values.size())
        return;
    this->_values[id] = value;
}
