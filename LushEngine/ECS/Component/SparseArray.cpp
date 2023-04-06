#include "ECS/Component/SparseArray.hpp"

using namespace Lush;

std::size_t SparseArray::getSize()
{
    return this->_values.size();
}

std::optional<std::any> &SparseArray::getValues(std::size_t id)
{
    if (id > this->_values.size())
        throw std::runtime_error("Incorrect ID");
    return this->_values[id];
}

void SparseArray::addValue(std::size_t id, std::any &value)
{
    if (id >= this->_values.size())
        this->_values.resize(id + 1);
    this->_values[id] = value;
}

void SparseArray::removeValue(std::size_t id)
{
    if (id >= this->_values.size())
        return;
    this->_values[id] = std::nullopt;
}

// void SparseArray::updateValue(std::size_t id, std::any &value)
// {
//     if (id >= this->_values.size())
//         return;
//     this->_values[id] = value;
// }
