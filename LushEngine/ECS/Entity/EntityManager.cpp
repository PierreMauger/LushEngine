#include "ECS/Entity/EntityManager.hpp"

using namespace Lush;

void EntityManager::addEntity(Entity &entity)
{
    this->_entities[this->_entities.size()] = entity;
}

void EntityManager::addEntity(Entity &entity, std::size_t index)
{
    this->_entities[index] = entity;
}

void EntityManager::removeEntity(Entity &entity)
{
    auto it = std::ranges::find_if(this->_entities, [&entity](const auto &pair) {
        return pair.second == entity;
    });

    if (it != this->_entities.end()) {
        this->_entities.erase(it);
    }
}

void EntityManager::removeEntity(const std::string &name)
{
    auto it = std::ranges::find_if(this->_entities, [&name](const auto &pair) {
        return pair.second.getName() == name;
    });

    if (it != this->_entities.end()) {
        this->_entities.erase(it);
    }
}

void EntityManager::removeEntity(std::size_t index)
{
    auto it = this->_entities.find(index);

    if (it != this->_entities.end()) {
        this->_entities.erase(it);
    }
}

bool EntityManager::hasEntity(const std::string &name)
{
    for (auto it = this->_entities.begin(); it != this->_entities.end(); it++) {
        if (it->second.getName() == name)
            return true;
    }
    return false;
}

bool EntityManager::hasEntity(std::size_t index)
{
    return this->_entities.find(index) != this->_entities.end();
}

Entity &EntityManager::getEntity(const std::string &name)
{
    for (auto it = this->_entities.begin(); it != this->_entities.end(); it++) {
        if (it->second.getName() == name)
            return it->second;
    }
    throw std::runtime_error("getEntity: Entity not found " + name);
}

Entity &EntityManager::getEntity(std::size_t index)
{
    if (this->_entities.find(index) == this->_entities.end())
        throw std::runtime_error("getEntity: Entity not found " + std::to_string(index));
    return this->_entities[index];
}

std::map<std::size_t, Entity> &EntityManager::getEntities()
{
    return this->_entities;
}

void EntityManager::clear()
{
    this->_entities.clear();
}

void EntityManager::clone(const EntityManager &other)
{
    this->_entities.clear();
    for (auto &[index, entity] : other._entities)
        this->_entities[index].clone(entity);
}
