#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include "ECS/Component/ComponentManager.hpp"

namespace Lush
{
    enum InfoEntity {
        POS = 0b001,
        VEL = 0b010,
        IDMODEL = 0b100
    };

    class EntityManager
    {
        private:
            std::vector<std::optional<std::size_t>> _masks;

        public:
            EntityManager();
            ~EntityManager() = default;

            std::vector<std::optional<std::size_t>> &getMasks();
            void addMask(std::size_t id, std::optional<std::size_t> mask);
            void removeMask(std::size_t id);
            void updateMask(std::size_t id, std::optional<std::size_t> mask);
    };
}

#endif // ENTITYMANAGER_HPP
