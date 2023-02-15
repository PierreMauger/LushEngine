#include "UUID.hpp"

using namespace Lush;

UUID::UUID()
{
    std::random_device rd;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<unsigned int> distr;

    unsigned long long now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    unsigned int uuid_high = distr(eng);
    unsigned int uuid_low = distr(eng);

    this->_id = (((unsigned long long)uuid_high) << 32) | (now << 32) | uuid_low;
}

std::string UUID::to_string() const
{
    std::stringstream ss;
    ss << std::hex << this->_id;
    return ss.str();
}

bool UUID::operator==(const UUID &other) const
{
    return this->_id == other._id;
}
