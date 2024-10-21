#include "pch.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class PoemsCollection {
private:
    std::vector<std::string> poems;
    std::shared_ptr<spdlog::logger> logger;

public:
    PoemsCollection() {
        if (!spdlog::get("PoemsCollection")) {
            logger = spdlog::stdout_color_mt("PoemsCollection");
        }
        else {
            logger = spdlog::get("PoemsCollection");
        }
    }

    void addPoem(const std::string& poem) {
        if (poem.empty()) {
            logger->warn("Attempting to add an empty poem");
        }
        else {
            logger->info("Adding poem: {}", poem);
        }
        poems.push_back(poem);
    }

    void sortPoemsByLength() {
        logger->info("Sorting poems by length");
        if (poems.empty()) {
            logger->warn("No poems to sort");
        }
        std::sort(poems.begin(), poems.end(), [](const std::string& a, const std::string& b) {
            return a.length() > b.length(); 
            });
    }

    std::vector<std::string> getPoems() const {
        logger->info("Retrieving poems");
        return poems;
    }
};

TEST(PoemsCollectionTest, SortByLengthTest) {
    PoemsCollection myPoems;
    myPoems.addPoem("One");
    myPoems.addPoem("Two");
    myPoems.addPoem("Three");

    myPoems.sortPoemsByLength();

    std::vector<std::string> sortedPoems = myPoems.getPoems();

    EXPECT_EQ(sortedPoems[0], "Three");
    EXPECT_EQ(sortedPoems[1], "One");
    EXPECT_EQ(sortedPoems[2], "Two");
}

TEST(PoemsCollectionTest, SortByLengthEmptyCollectionTest) {
    PoemsCollection myPoems;
    myPoems.addPoem("");

    myPoems.sortPoemsByLength();

    std::vector<std::string> sortedPoems = myPoems.getPoems();

    EXPECT_FALSE(sortedPoems.empty());
}

TEST(PoemsCollectionTest, SortByLengthErrorTest) {
    PoemsCollection myPoems;
    myPoems.addPoem("Four");
    myPoems.addPoem("Five");

    myPoems.sortPoemsByLength();

    std::vector<std::string> sortedPoems = myPoems.getPoems();

    EXPECT_EQ(sortedPoems[0], "Four");
}

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::info);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}