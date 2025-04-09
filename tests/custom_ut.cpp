
#include <unrolled_list.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(InsertWork, singleInsert) {
    unrolled_list<int> list;
    list.insert(list.begin(), 42);
    
    EXPECT_EQ(list.size(), 1u);

    EXPECT_EQ(*list.begin(), 42);
}

TEST(InsertWork, multipleInsert) {
    unrolled_list<int> list;
    
    list.insert(list.begin(), 10);
    list.insert(list.begin(), 20);
    list.insert(list.begin(), 30);

    EXPECT_EQ(list.size(), 3u);

    auto it = list.begin();
    EXPECT_EQ(*it++, 30);
    EXPECT_EQ(*it++, 20);
    EXPECT_EQ(*it++, 10);
}

TEST(NodeAllcTraits, iterationCheck) {
    unrolled_list<int> list;
    list.insert(list.end(), 1);
    list.insert(list.end(), 2);
    list.insert(list.end(), 3);

    std::vector<int> collected;
    for (auto& val : list) {
        collected.push_back(val);
    }

    ASSERT_EQ(collected.size(), 3u);
    EXPECT_EQ(collected[0], 1);
    EXPECT_EQ(collected[1], 2);
    EXPECT_EQ(collected[2], 3);
}

TEST(NodeAllcTraits, eraseCheck) {
    unrolled_list<int> list;
    list.insert(list.end(), 5);
    list.insert(list.end(), 10);
    list.insert(list.end(), 15);

    auto it = list.begin();
    ++it; 
    it = list.erase(it);

    EXPECT_EQ(*it, 15);
    EXPECT_EQ(list.size(), 2);

    std::vector<int> collected;
    for (auto& val : list) {
        collected.push_back(val);
    }
    EXPECT_THAT(collected, ::testing::ElementsAre(5, 15));
}

TEST(ErasePopWork, pushPopEdgeCase) {
    unrolled_list<int> list;
    list.insert(list.end(), 100);
    list.insert(list.end(), 200);

    EXPECT_EQ(list.size(), 2);

    auto it = list.end();
    --it;
    list.erase(it);

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(*list.begin(), 100);
}

TEST(PopFrontWork, singlePopFront) {
    unrolled_list<int> list;
    list.push_back(10);

    ASSERT_EQ(list.size(), 1u);
    list.pop_front();
    
    EXPECT_EQ(list.size(), 0u);
    EXPECT_TRUE(list.empty());
}

TEST(PopFrontWork, multiplePopFront) {
    unrolled_list<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    
    ASSERT_EQ(list.size(), 3u);

    list.pop_front();
    EXPECT_EQ(*list.begin(), 2);
    EXPECT_EQ(list.size(), 2u);

    list.pop_front();
    auto it = list.begin();
    EXPECT_EQ(*it, 3);
    EXPECT_EQ(list.size(), 1u);

    list.pop_front(); 
    EXPECT_TRUE(list.empty());
}

TEST(PopBackWork, singlePopBack) {
    unrolled_list<int> list;
    list.push_back(42);

    ASSERT_EQ(list.size(), 1u);
    list.pop_back();

    EXPECT_EQ(list.size(), 0u);
    EXPECT_TRUE(list.empty());
}

TEST(PopBackWork, multiplePopBack) {
    unrolled_list<int> list;
    list.push_back(5);
    list.push_back(10);
    list.push_back(15);

    ASSERT_EQ(list.size(), 3u);

    list.pop_back();
    EXPECT_EQ(list.size(), 2u);

    list.pop_back(); 
    EXPECT_EQ(list.size(), 1u);

    list.pop_back(); 
    EXPECT_TRUE(list.empty());
}

