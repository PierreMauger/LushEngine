#ifndef UUID_HPP
#define UUID_HPP

#include "Includes.hpp"

namespace Lush
{
    class UUID
    {
        public:
            UUID();
            std::string to_string() const;

            bool operator==(const UUID &other) const;

        private:
            unsigned long long _id;
    };

}

#endif // UUID_HPP
