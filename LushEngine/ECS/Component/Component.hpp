#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Includes.hpp"

namespace Lush
{
    class Component
    {
        private:
            std::vector<std::optional<std::any>> _values;

        public:
            Component();
            ~Component() = default;

            std::size_t getSize();
            std::optional<std::any> &getValues(std::size_t id);
            void addValue(std::size_t id, std::any value);
            void removeValue(std::size_t id);
            void updateValue(std::size_t id, std::any value);
    };
}

#endif // COMPONENT_HPP
