#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include "ECS/Entity/Entity.hpp"

namespace Lush
{
    class EntityManager
    {
        private:
            std::map<std::size_t, Entity> _entities;

        public:
            EntityManager() = default;
            ~EntityManager() = default;

            void addEntity(Entity &entity);
            void addEntity(Entity &entity, std::size_t index);
            void removeEntity(Entity &entity);
            void removeEntity(const std::string &name);
            void removeEntity(std::size_t index);

            bool hasEntity(const std::string &name);
            bool hasEntity(std::size_t index);

            Entity &getEntity(const std::string &name);
            Entity &getEntity(std::size_t index);

            std::map<std::size_t, Entity> &getEntities();

            void clear();
    };
}

#endif // ENTITY_MANAGER_HPP
