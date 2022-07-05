#ifndef SAFE_QUEUE_HPP
#define SAFE_QUEUE_HPP

#include "Includes.hpp"

namespace Lush
{
    template <typename T> class SafeQueue
    {
        private:
            std::queue<T> _queue;
            mutable std::mutex _mutex;

        public:
            SafeQueue() = default;
            SafeQueue(const SafeQueue &) = delete;
            SafeQueue(SafeQueue &&) noexcept {};
            std::size_t size() const;
            void push(const T &);
            void pop();
            T &front();
    };
}

#endif // SAFE_QUEUE_HPP
