#ifndef SPARSE_ARRAY_HPP
#define SPARSE_ARRAY_HPP

#include "Includes.hpp"

namespace Lush
{
    class SparseArray
    {
        private:
            std::vector<std::optional<std::any>> _values;

        public:
            SparseArray() = default;
            ~SparseArray() = default;

            std::size_t getSize();
            std::optional<std::any> &getValues(std::size_t id);
            void addValue(std::size_t id, std::any &value);
            void removeValue(std::size_t id);
            // void updateValue(std::size_t id, std::any &value);
    };
}

#endif // SPARSE_ARRAY_HPP
