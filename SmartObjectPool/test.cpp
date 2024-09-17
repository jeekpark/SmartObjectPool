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
    std::cout << "Pool Left Object: " << pool.GetAvailableObjectCount() << std::endl << std::endl;

    auto obj = pool.Acquire();
    std::cout << "obj acquired" << std::endl;
    std::cout << "Pool Left Object: " << pool.GetAvailableObjectCount() << std::endl << std::endl;

    obj.reset();
    std::cout << "obj reset(return, release)" << std::endl;
    std::cout << "Pool Left Object: " << pool.GetAvailableObjectCount() << std::endl << std::endl;

    return 0;
}