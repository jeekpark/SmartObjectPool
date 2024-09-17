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
        mStack.reserve(PoolSize);
        for (size_t i = 0; i < PoolSize; ++i)
        {
            auto obj = std::make_unique<T>(std::forward<Args>(args)...);
            mStack.push(obj.get());
            mPointersToDelete.push_back(std::move(obj));
        }
    }
    
    Pool(const Pool&) = delete;
    Pool(Pool&&) = default;

    Pool& operator=(const Pool&) = delete;
    Pool& operator=(Pool&&) = default;

    auto Acquire() noexcept
    {
        auto deleter = [this](T* obj)
            {
                if (obj != nullptr)
                {
                    mStack.push(obj);
                }
            };

        if (mStack.empty())
        {
            return std::unique_ptr<T, decltype(deleter)>(nullptr, deleter);
        }

        T* obj = mStack.top();
        mStack.pop();
        return std::unique_ptr<T, decltype(deleter)>(obj, deleter);
    }

    size_t GetAvailableObjectCount() const noexcept { return mStack.size(); }

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

    ReservableStack<T*> mStack;
    std::vector<std::unique_ptr<T>> mPointersToDelete;
};
