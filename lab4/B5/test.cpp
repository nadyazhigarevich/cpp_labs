#include "pch.h"

#include <iostream>
#include <set>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>



std::shared_ptr<spdlog::logger> g_logger = spdlog::stdout_color_mt("NumberCollection");

class NumberCollection {
private:
    std::set<int> numbers;

public:
    NumberCollection() {
        g_logger->info("New number collection created");
    }

    // Add a number
    void add(int number) {
        numbers.insert(number);
        g_logger->info("Added number: {}", number);
    }

    // Remove a number
    void remove(int number) {
        auto it = numbers.find(number);
        if (it != numbers.end()) {
            numbers.erase(it);
            g_logger->info("Removed number: {}", number);
        }
        else {
            g_logger->warn("Attempted to remove non-existent number: {}", number);
        }
    }

    // Find the closest number to the target
    int findClosest(int target) {
        if (numbers.empty()) {
            g_logger->error("Attempt to find closest number in an empty collection");
            throw std::runtime_error("Collection is empty");
        }

        auto it = numbers.lower_bound(target);

        if (it == numbers.end()) {
            return *(--it); // Return the last element
        }
        if (it == numbers.begin()) {
            return *it; // Return the first element
        }

        // Compare with the previous element
        int closest = *it;
        if (std::abs(target - closest) >= std::abs(target - *(--it))) {
            closest = *it;
        }

        g_logger->info("Found closest number {} to target {}", closest, target);
        return closest;
    }
};

class NumberCollectionTest : public ::testing::Test {
protected:
    NumberCollection collection;

    void SetUp() override {
        collection.add(10);
        collection.add(5);
        collection.add(20);
    }
};

TEST_F(NumberCollectionTest, AddNumber) {
    collection.add(15);
    EXPECT_EQ(collection.findClosest(15), 15);
}

TEST_F(NumberCollectionTest, RemoveNumber) {
    collection.remove(10);
    EXPECT_EQ(collection.findClosest(12), 5); // Closest to 12 after removing 10
}

TEST_F(NumberCollectionTest, FindClosestNumber) {
    EXPECT_EQ(collection.findClosest(12), 10); // Closest to 12
    EXPECT_EQ(collection.findClosest(21), 20); // Closest to 21
    EXPECT_EQ(collection.findClosest(4), 5);   // Closest to 4
}

TEST_F(NumberCollectionTest, EmptyCollection) {
    NumberCollection emptyCollection;
    EXPECT_THROW(emptyCollection.findClosest(10), std::runtime_error);
}

TEST_F(NumberCollectionTest, EdgeCases) {
    collection.add(30);
    EXPECT_EQ(collection.findClosest(25), 20); // Closest to 25
    collection.remove(20);
    EXPECT_EQ(collection.findClosest(25), 30); // Closest to 25 after removing 20
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}