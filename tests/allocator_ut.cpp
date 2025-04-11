#include <unrolled_list.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class NodeTag {};

class SomeObj1 {
public:
    static inline int ConstructorCalled = 0;
    static inline int DestructorCalled = 0;

    SomeObj1() {
        ++ConstructorCalled;
    }

    ~SomeObj1() {
        ++DestructorCalled;
    }
};


template<class Alloc>
concept AllocatorRequirements = requires(Alloc alloc, std::size_t n)
{
    { *alloc.allocate(n) } -> std::same_as<typename Alloc::value_type&>;
    { alloc.deallocate(alloc.allocate(n), n) };
} && std::copy_constructible<Alloc>
  && std::equality_comparable<Alloc>;


template<typename T>
class TestAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using size_type = size_t;
    using is_always_equal = std::true_type;

    static inline int AllocationCount = 0;
    static inline int ElementsAllocated = 0;

    TestAllocator() = default;

    template<typename U>
    TestAllocator(const TestAllocator<U>& other) {
    }

    pointer allocate(size_type sz) {
        if constexpr (std::is_same_v<T, SomeObj1>) {
            ++TestAllocator<SomeObj1>::AllocationCount;
            TestAllocator<SomeObj1>::ElementsAllocated += sz;
        } else {
            ++TestAllocator<NodeTag>::AllocationCount;
            TestAllocator<NodeTag>::ElementsAllocated += sz;
        }
        return reinterpret_cast<pointer>(new char[sz * sizeof(value_type)]);
    }

    void deallocate(pointer p, std::size_t n) {

    }

    bool operator==(const TestAllocator& other) const {
        return true;
    }

};


static_assert(AllocatorRequirements<TestAllocator<SomeObj1>>);

class WorkWithAllocatorTest : public testing::Test {
public:
    void SetUp() override {
        SomeObj1::ConstructorCalled = 0;
        SomeObj1::DestructorCalled = 0;

        TestAllocator<SomeObj1>::AllocationCount = 0;
        TestAllocator<SomeObj1>::ElementsAllocated = 0;

        TestAllocator<NodeTag>::AllocationCount = 0;
        TestAllocator<NodeTag>::ElementsAllocated = 0;
    }

};

/*
    В тесте задаётся NodeMaxSize = 5, а далее добавляется 11 элементов.

    Ожидается, что будет:
        1. 3 аллокации Node
        2. 3 создания Node
        3. 11 конструкторов и деструкторов у SomeObj1
*/
TEST_F(WorkWithAllocatorTest, simplePushBack) {
    TestAllocator<SomeObj1> allocator;
    unrolled_list<SomeObj1, 5, TestAllocator<SomeObj1>> list(allocator);
    for (int i = 0; i < 11; ++i) {
        list.push_back(SomeObj1{});
    }

    ASSERT_EQ(TestAllocator<NodeTag>::AllocationCount, 3);
    ASSERT_EQ(TestAllocator<NodeTag>::ElementsAllocated, 3);

    ASSERT_EQ(TestAllocator<SomeObj1>::AllocationCount, 0);

    ASSERT_EQ(SomeObj1::ConstructorCalled, 11);
    ASSERT_EQ(SomeObj1::DestructorCalled, 11);
}
