#pragma once

#include <memory>
#include <stdexcept>
#include <stack>
#include <vector>

template <typename T, size_t PoolSize>
class Pool
{
public:
    template <typename... Args>
    Pool(Args&&... args)
    {
        mAvailableObjects.reserve(PoolSize);
        for (size_t i = 0; i < PoolSize; ++i)
        {
            auto obj = std::make_unique<T>(std::forward<Args>(args)...);
            mAvailableObjects.push(obj.get());
            mPoolStorage.push_back(std::move(obj));
        }
    }

    Pool(Pool&&) = default;
    Pool& operator=(Pool&&) = default;
    
    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;
    

    auto Acquire() noexcept
    {
        auto deleter = [this](T* obj)
            {
                if (obj != nullptr)
                {
                    mAvailableObjects.push(obj);
                }
            };

        if (mAvailableObjects.empty())
        {
            return std::unique_ptr<T, decltype(deleter)>(nullptr, deleter);
        }

        T* obj = mAvailableObjects.top();
        mAvailableObjects.pop();
        return std::unique_ptr<T, decltype(deleter)>(obj, deleter);
    }

    size_t GetAvailableObjectCount() const noexcept { return mAvailableObjects.size(); }

private:

    template<typename T>
    class ReservableStack : public std::stack<T, std::vector<T>>
    {
    public:
        void reserve(size_t n) { this->c.reserve(n); }
        size_t capacity() const { return this->c.capacity(); }
        size_t size() const { return this->c.size(); }
        bool empty() const { return this->c.empty(); }
    };

    ReservableStack<T*> mAvailableObjects;
    std::vector<std::unique_ptr<T>> mPoolStorage;
};
