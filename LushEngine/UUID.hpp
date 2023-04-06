#ifndef UUID_HPP
#define UUID_HPP

#include "Includes.hpp"

namespace Lush
{
    class UUID
    {
        private:
            unsigned long long _id;

        public:
            UUID();
            [[nodiscard]] std::string toString() const;

            bool operator==(const UUID &other) const;
    };

}

#endif // UUID_HPP
