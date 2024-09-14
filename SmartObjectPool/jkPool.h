#pragma once


#include <array>
#include <memory>
#include <bitset>
#include <stdexcept>

template <typename T, size_t N>
class Pool
{
public:
    template <typename... Args>
    Pool(Args&&... args)
        : mLeftObjectCount(N)
        , mArr{ std::forward<Args>(args)... }
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

        for (size_t i = 0; begin != end; ++begin, ++i)
        {
            mArr[i] = *begin;
            mStatus.set(i);
        }
    }

private:
    size_t mLeftObjectCount;
    std::array<T, N> mArr;
    std::bitset<N> mStatus;
};
