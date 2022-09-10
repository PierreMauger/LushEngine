#include "Packet.hpp"

using namespace Lush;

std::vector<std::byte> &Packet::getData()
{
    return this->_data;
}

bool Packet::empty() const
{
    return this->_data.empty();
}

void Packet::clear()
{
    this->_data.clear();
}

void Packet::append(const void *data, std::size_t size)
{
    this->_data.insert(this->_data.end(), static_cast<const std::byte *>(data), static_cast<const std::byte *>(data) + size);
}

Packet &Packet::operator<<(const std::string &data)
{
    *this << data.size();

    this->append(data.c_str(), data.size());
    return *this;
}

Packet &Packet::operator>>(std::string &data)
{
    std::size_t size = 0;

    *this >> size;
    data.resize(size);
    std::memcpy(data.data(), &this->_data[0], size);
    this->_data.erase(this->_data.begin(), this->_data.begin() + size);
    return *this;
}
