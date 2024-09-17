#pragma once

#include <array>
#include <memory>
#include <bitset>
#include <stdexcept>
#include <iterator>

template <typename T, size_t N>
class Pool
{
public:
    template <typename... Args>
    Pool(Args&&... args)
        : mAvailableObjectCount(N)
        , mArr{ T{ std::forward<Args>(args)... } }
        , mStatus{}
    {
        mStatus.set();
    }

    template <typename Iterator>
    Pool(Iterator begin, Iterator end)
        : mAvailableObjectCount(N)
        , mArr{}
        , mStatus{}
    {
        if (std::distance(begin, end) != N)
        {
            throw std::invalid_argument("Invalid range");
        }

        std::move(begin, end, mArr.begin());
        mStatus.set();
    }

    ~Pool() = default;

    Pool(const Pool&) = delete;
    Pool(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;
    Pool& operator=(std::initializer_list<T>) = delete;
    Pool& operator=(std::array<T, N>) = delete;
    
    auto Acquire() noexcept
    {
        auto deleter = [this](const T* obj)
            {
                if (obj != nullptr)
                {
                    mStatus.set(obj -&mArr[0]);
                    ++mAvailableObjectCount;
                }
            };

        if (mAvailableObjectCount == 0)
        {
            return std::unique_ptr<T, decltype(deleter)>(nullptr, deleter);
        }

        for (size_t i = 0; i < mStatus.size(); ++i)
        {
            if (mStatus[i])
            {
                mStatus.reset(i);
                --mAvailableObjectCount;
                return std::unique_ptr<T, decltype(deleter)>(&mArr[i], deleter);
            }
        }

        return std::unique_ptr<T, decltype(deleter)>(nullptr, deleter);
    }

    size_t GetAvailableObjectCount() const noexcept { return mAvailableObjectCount; }

private:

    template<typename T>
    class AccessibleStack : public std::stack<std::unique_ptr<T>, std::vector<std::unique_ptr<T>>>
    {
    public:
        using std::stack<std::unique_ptr<T>, std::vector<std::unique_ptr<T>>>::c;
    };

    size_t mAvailableObjectCount;
    std::array<T, N> mArr;
    std::bitset<N> mStatus;
};
