#include "SafeQueue.hpp"

using namespace Lush;

template <typename T> std::size_t SafeQueue<T>::size() const
{
    std::lock_guard<std::mutex> lock(this->_mutex);
    return this->_queue.size();
}

template <typename T> void SafeQueue<T>::push(const T &value)
{
    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_queue.push(value);
}

template <typename T> void SafeQueue<T>::pop()
{
    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_queue.pop();
}

template <typename T> T &SafeQueue<T>::front()
{
    std::lock_guard<std::mutex> lock(this->_mutex);
    return this->_queue.front();
}
