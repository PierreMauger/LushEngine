#ifndef COMPONENT_MANAGER_HPP
#define COMPONENT_MANAGER_HPP

#include "ECS/Component/SparseArray.hpp"
#include "Includes.hpp"

namespace Lush
{
    class ComponentManager
    {
        private:
            std::map<std::type_index, SparseArray> _componentArray;
            std::map<std::size_t, std::type_index> _orderedMap;

            std::unordered_map<std::string, SparseArray> _instanceFields;

        public:
            ComponentManager() = default;
            ~ComponentManager() = default;

            std::map<std::type_index, SparseArray> &getComponentArray();
            std::type_index getComponentType(std::size_t index);

            template <typename T> T &getComponent(std::size_t id)
            {
                return std::any_cast<T &>(this->_componentArray[typeid(T)].getValues(id).value());
            }

            template <typename T> void bindComponent()
            {
                this->_componentArray[std::type_index(typeid(T))] = SparseArray();
                this->_orderedMap.try_emplace(this->_orderedMap.size(), std::type_index(typeid(T)));
            }

            template <typename T> void addComponent(std::size_t id, T value = T())
            {
                std::any temp = value;
                this->_componentArray[std::type_index(typeid(T))].addValue(id, temp);
            }

            // void addEntity(std::size_t id);
            void removeSingleComponent(std::size_t id, std::type_index type);
            void removeAllComponents(std::size_t id);

            // void updateComponent(std::size_t id);

            template <typename T> T &getInstanceField(const std::string &name, const std::string &fieldName, std::size_t id)
            {
                auto &fields = std::any_cast<std::unordered_map<std::string, std::any> &>(this->_instanceFields[name].getValues(id).value());
                return std::any_cast<T &>(fields[fieldName]);
            }

            std::unordered_map<std::string, std::any> &getInstanceFields(const std::string &name, std::size_t id);
            SparseArray &getAllInstanceFields(const std::string &name);
            void bindInstanceFields(const std::string &name);
            void addInstanceFields(const std::string &name, std::size_t id, std::any value);
            void removeInstanceFields(const std::string &name, std::size_t id);

            ComponentManager &operator=(const ComponentManager &other);
    };
}

#endif // COMPONENT_MANAGER_HPP
