#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "ECS/Component/Component.hpp"
#include "ECS/Component/ScriptComponent.hpp"
#include "Includes.hpp"
#include "UUID.hpp"

namespace Lush
{
    class Entity
    {
        private:
            std::string _uuid;
            std::string _name;
            std::unordered_map<std::type_index, std::shared_ptr<Component>> _components;
            std::unordered_map<std::string, ScriptComponent> _scriptComponents;

        public:
            Entity();
            ~Entity() = default;

            std::string getName() const;
            void setName(const std::string &name);

            template <typename T> T &getComponent()
            {
                auto it = this->_components.find(typeid(T));
                if (it != this->_components.end()) {
                    return static_cast<T &>(*it->second.get());
                }
                throw std::runtime_error("Component not found");
            }

            template <typename T> void addComponent(const T &component)
            {
                this->_components[typeid(T)] = std::make_shared<T>(component);
            }

            template <typename T> void removeComponent()
            {
                this->_components.erase(typeid(T));
            }

            template <typename T> bool hasComponent()
            {
                return this->_components.find(typeid(T)) != this->_components.end();
            }

            ScriptComponent &getScriptComponent(const std::string &className);
            void addScriptComponent(const std::string &className, ScriptComponent scriptComponent);
            void removeScriptComponent(const std::string &className);
            bool hasScriptComponent(const std::string &className);

            bool operator==(const Entity &other) const;
    };
}

#endif // ENTITY_HPP
