#ifndef SCRIPT_COMPONENT_HPP
#define SCRIPT_COMPONENT_HPP

#include "Includes.hpp"

namespace Lush
{
    class ScriptComponent
    {
        private:
            std::unordered_map<std::string, std::any> _fields;

        public:
            ScriptComponent() = default;
            ~ScriptComponent() = default;

            template <typename T> T &getField(const std::string &name)
            {
                auto it = this->_fields.find(name);
                if (it != this->_fields.end())
                    return std::any_cast<T &>(it->second);
                throw std::runtime_error("Field " + name + " not found in script component");
            }

            std::unordered_map<std::string, std::any> &getFields();
            void addField(const std::string &name, std::any value);
            void removeField(const std::string &name);
            bool hasField(const std::string &name);
    };
}

#endif // SCRIPT_COMPONENT_HPP
