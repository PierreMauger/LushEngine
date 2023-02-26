#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include "ECS/Component/ComponentManager.hpp"

namespace Lush
{
    class EntityManager
    {
        private:
            std::vector<std::optional<std::size_t>> _masks;
            std::map<std::size_t, std::vector<std::size_t>> _maskCategory;

        public:
            EntityManager();
            ~EntityManager() = default;

            std::vector<std::optional<std::size_t>> &getMasks();

            void addMaskCategory(std::size_t category);
            std::vector<std::size_t> &getMaskCategory(std::size_t category);

            void addMask(std::size_t id, std::optional<std::size_t> mask);
            void removeMask(std::size_t id);
            void updateMask(std::size_t id, std::optional<std::size_t> mask);

            bool hasMask(std::size_t id, std::size_t mask);
    };
}

#endif // ENTITYMANAGER_HPP
