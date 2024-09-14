#include "jkPool.h"

#include <iostream>

class Test
{
public:
    Test() = default;

    Test(int a)
        : mValue(a)
    {
    }
    ~Test()
    {
    }

    int GetValue() const
    {
        return mValue;
    }
private:
    int mValue;

};

int main()
{
    Pool<Test, 5> pool{ 314 };
    std::cout << "Pool Created" << std::endl;
    std::cout << "Pool Left Object: " << pool.GetLeftObjectCount() << std::endl;

    auto obj = pool.Acquire();
    std::cout << "obj acquired" << std::endl;
    std::cout << "Pool Left Object: " << pool.GetLeftObjectCount() << std::endl;

    obj.reset();
    std::cout << "obj reset" << std::endl;
    std::cout << "Pool Left Object: " << pool.GetLeftObjectCount() << std::endl;

    return 0;
}