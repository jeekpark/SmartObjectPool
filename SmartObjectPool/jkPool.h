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
        : mLeftObjectCount(N)
        , mArr{ T{ std::forward<Args>(args)... } }
        , mStatus{}
    {
        mStatus.set();
    }

    template <typename Iterator>
    Pool(Iterator begin, Iterator end)
        : mLeftObjectCount(N)
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
                    ++mLeftObjectCount;
                }
            };

        if (mLeftObjectCount == 0)
        {
            return std::unique_ptr<T, decltype(deleter)>(nullptr, deleter);
        }

        for (size_t i = 0; i < mStatus.size(); ++i)
        {
            if (mStatus[i])
            {
                mStatus.reset(i);
                --mLeftObjectCount;
                return std::unique_ptr<T, decltype(deleter)>(&mArr[i], deleter);
            }
        }

        return std::unique_ptr<T, decltype(deleter)>(nullptr, deleter);
    }

    size_t GetLeftObjectCount() const noexcept { return mLeftObjectCount; }

private:
    size_t mLeftObjectCount;
    std::array<T, N> mArr;
    std::bitset<N> mStatus;
};
