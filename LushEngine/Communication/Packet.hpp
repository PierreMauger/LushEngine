#ifndef PACKET_HPP
#define PACKET_HPP

#include "Includes.hpp"

namespace Lush
{
    class Packet
    {
        private:
            std::vector<std::byte> _data;

        public:
            Packet() = default;
            Packet(const Packet &other) = default;
            ~Packet() = default;

            std::vector<std::byte> &getData();
            bool empty() const;
            void clear();
            void append(const void *data, std::size_t size);

            template <typename T> Packet &operator<<(const T &data)
            {
                this->append(&data, sizeof(T));
                return *this;
            }
            Packet &operator<<(const std::string &data);

            template <typename T> Packet &operator>>(T &data)
            {
                if (this->_data.size() < sizeof(T))
                    throw std::runtime_error("Packet::operator>>: Not enough data in packet.");

                data = *reinterpret_cast<T *>(&this->_data[0]);
                this->_data.erase(this->_data.begin(), this->_data.begin() + sizeof(T));
                return *this;
            }
            Packet &operator>>(std::string &data);
    };
}

#endif // PACKET_HPP
