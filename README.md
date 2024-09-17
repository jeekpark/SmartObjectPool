# SmartObjectPool

C++의 스마트포인터(unique_ptr)을 이용한 오브젝트풀 ```header-only``` 라이브러리입니다. 해당 라이브러리는 게임, 리얼타임 렌더링, 고성능 프로그래밍에서 메모리 할당과 해제를 줄이기 위해 고정된 수 의 개체를 미리 생성하고, 필요할 때 획득(acquire)하고, 사용후 유니크 포인터를 통해 자동 반납(return, release)하도록 설계되어있습니다. 고정된 크기의 오브젝트풀을 제공하기 때문에 초기화시 필요한 양만큼 미리 생성하고, 이후에는 오브젝트풀의 크기를 조절할 수 없는 정적(static)인 구조입니다.

해당 코드는 교육/학습의 목표를 가지고 설계된 코드입니다. 학습자들에게는 코드 리딩을 추천드립니다.
개선 사항이 있다면 Issue 부탁드립니다~!

# 주요 특징
- **static 크기**: 정적 크기 오브젝트풀로 설계되어 있어 메모리 할당을 반복(재할당)하지 않습니다. 오브젝트풀의 크기는 초기화시 지정하고, 이후에는 고정되어있어 사용시 유의가 필요합니다.
- **static 타입**: 오브젝트 풀에 저장할 수 있는 객체의 타입이 컴파일 타임에 고정됩니다. 객체의 타입이 고정되어 있기 때문에, 컴파일 시점에 타입 오류를 쉽게 잡아낼 수 있습니다.
- **noexcept**: 모든 함수(API)는 noexcept로 선언되어있습니다. try-catch, throw, exception 구조는 성능저하를 야기함으로 예외상황은 unique_ptr의 nullptr 반환으로 처리되었습니다. 단 생성자는 반환값이 없기때문에 예외 throw를 발생시킵니다. 생성자가 작동되는 시점은 게임의 로딩씬과 같이 성능저하/부하가 걸려도 무방한 코드에 배치하시기를 권장합니다.
- **자동반납**: unique_ptr의 커스텀 해제(```deleter```)를 통해 객체의 반환을 자동으로 처리합니다. 따라서 ```Acquire()```함수로 객체를 unique_ptr로 획득 한 후, 객체를 사용하고 반납할 때, unique_ptr에서 해제만 해주면 자동으로 반납이 되어 간결한 사용이 가능합니다.

# 코드설명

## 기본사용
```cpp
Pool<Test, 6> pool;
auto obj = pool.Acquire();
if (obj.get() != nullptr)
{
    // 객체 획득 성공
    obj->SomeFunction();
}
else
{
    // 객체 획득 실패
}

```
위 코드에서는 Pool<Test, 6>을 선언하여 크기가 6인 Test 오브젝풀이 생성됩니다. ```Acquire()``` 함수는 객체를 획득(풀에서 하나 꺼내오는)하는 역할을 하며, 만약 풀이 비어있으면 unique_ptr은 nullptr을 가진채로 반환합니다.

## 클래스 가변인자 초기화
```cpp
Pool<Foo, 6> pool{args1, args2}; // Foo의 생성자 인자를 넘김
```
위 코드는 ```Foo```객체를 생성하며 ```Foo```생성자의 인자를 넘겨 오브젝트풀이 객체를 초기화할 수 있도록 합니다.

## 자동 반납
```cpp
int main()
{
    Pool<TestObject, 3> pool;  // 크기가 3인 풀 생성

    // 객체를 풀에서 unique_ptr로 획득
    unique_ptr<TestObject> obj1 = pool.Acquire();

    // 객체 사용
    obj1->SomeFunction();

    // 객체를 자동으로 풀에 반납
    obj1.reset();

    return 0;
}
```
위 코드는 객체를 반납하는 코드입니다. unique_ptr가 스코프를 벗어나거나(unique_ptr의 RAII 특성 참고) 명시적으로 ```reset()```이 호출될 때, 커스텀 ```deleter```가 호출되어 자동으로 오브젝트풀에 객체가 반납됩니다.
객체를 반납하면 오브젝트풀은 해당 객체를 다시 사용할 수 있는 상태로 변합니다. 커스텀 해제 코드는 ```jkPool.h```참고
