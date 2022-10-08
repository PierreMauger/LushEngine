#ifndef COMPONENTMANAGER_HPP
#define COMPONENTMANAGER_HPP

#include "Component/Component.hpp"
#include "Component/ComponentTypes.hpp"
#include "Includes.hpp"

namespace Lush
{
    class ComponentManager
    {
        private:
            std::map<std::type_index, Component> _componentArray;
            std::map<std::size_t, std::type_index> _orderedMap;

        public:
            ComponentManager();
            ~ComponentManager() = default;

            std::map<std::type_index, Component> &getComponentArray();
            Component &getComponent(std::type_index type);
            Component &getComponent(std::size_t index);
            std::type_index getComponentType(std::size_t index);

            template <typename T> void bindComponent()
            {
                this->_componentArray[std::type_index(typeid(T))] = Component();
                this->_orderedMap.try_emplace(this->_orderedMap.size(), std::type_index(typeid(T)));
            }

            template <typename T> void addComponent(std::size_t id)
            {
                this->_componentArray[std::type_index(typeid(T))].addValue(id, T());
            }

            void addEntity(std::size_t id);
            void removeSingleComponent(std::size_t id, std::type_index type);
            void removeAllComponents(std::size_t id);
            void updateComponent(std::size_t id);
    };
}

#endif // COMPONENTMANAGER_HPP
