#pragma once

#include <memory>
#include <stdexcept>
#include <stack>
#include <vector>
#include <cassert>

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
            T* obj = new T(args...);
            mAvailableObjects.push(obj);
            mAllPointersForDeletion.push_back(obj);
        }
    }

    ~Pool()
    {
        assert(mAvailableObjects.size() == PoolSize);
        for (auto obj : mAllPointersForDeletion)
        {
            delete obj;
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

        if (mAvailableObjects.size() == 0)
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
        size_t size() const { return this->c.size(); }
    };

    ReservableStack<T*> mAvailableObjects;
    std::vector<T*> mAllPointersForDeletion;
};
