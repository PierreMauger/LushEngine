#include "ECS/Entity/EntityManager.hpp"

using namespace Lush;

std::vector<std::optional<std::size_t>> &EntityManager::getMasks()
{
    return this->_masks;
}

void EntityManager::addMaskCategory(std::size_t category)
{
    this->_maskCategory[category] = std::vector<std::size_t>();
}

std::vector<std::size_t> &EntityManager::getMaskCategory(std::size_t category)
{
    return this->_maskCategory[category];
}

void EntityManager::addMask(std::size_t id, std::optional<std::size_t> mask)
{
    if (id >= this->_masks.size())
        this->_masks.resize(id + 1);
    this->_masks[id] = mask;

    for (auto &[key, value] : this->_maskCategory)
        if (mask.has_value() && (mask.value() & key) == key)
            value.push_back(id);
}

void EntityManager::removeMask(std::size_t id)
{
    if (id >= this->_masks.size())
        return;
    this->_masks[id] = std::nullopt;

    for (auto &[key, value] : this->_maskCategory)
        value.erase(std::remove(value.begin(), value.end(), id), value.end());
}

void EntityManager::updateMask(std::size_t id, std::optional<std::size_t> mask)
{
    if (id >= this->_masks.size())
        return;
    this->_masks[id] = mask;

    for (auto &[key, value] : this->_maskCategory) {
        value.erase(std::remove(value.begin(), value.end(), id), value.end());
        if (mask.has_value() && (mask.value() & key) == key)
            value.push_back(id);
    }
}

bool EntityManager::hasMask(std::size_t id, std::size_t mask)
{
    if (id >= this->_masks.size())
        return false;
    return this->_masks[id].has_value() && (this->_masks[id].value() & mask) == mask;
}

void EntityManager::clear()
{
    this->_masks.clear();
    for (auto &[id, category] : this->_maskCategory)
        category.clear();
}
