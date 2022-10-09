#include "ECS/Entity/EntityManager.hpp"

using namespace Lush;

EntityManager::EntityManager()
{
}

std::vector<std::optional<std::size_t>> &EntityManager::getMasks()
{
    return this->_masks;
}

void EntityManager::addMask(std::size_t id, std::optional<std::size_t> mask)
{
    if (id >= this->_masks.size())
        this->_masks.resize(id + 1);
    this->_masks[id] = mask;
}

void EntityManager::removeMask(std::size_t id)
{
    if (id >= this->_masks.size())
        return;
    this->_masks[id] = std::nullopt;
}

void EntityManager::updateMask(std::size_t id, std::optional<std::size_t> mask)
{
    if (id >= this->_masks.size())
        return;
    this->_masks[id] = mask;
}
