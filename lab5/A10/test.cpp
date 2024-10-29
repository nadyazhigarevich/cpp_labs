#include "pch.h"

std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("logger");

class FileHandler {
public:
    static std::map<char, int> readCharacters(const std::string& filename) {
        std::ifstream file(filename);
        std::map<char, int> charCount;

        if (!file.is_open()) {
            logger->error("File not found or could not be opened: {}", filename);
            return charCount;
        }

        char c;
        if (file.peek() == std::ifstream::traits_type::eof()) {
            logger->warn("File is empty: {}", filename);
            return charCount;
        }

        while (file.get(c)) {
            charCount[c]++;
        }
        file.close();
        logger->info("Successfully counted characters in file: {}", filename);

        return charCount;
    }
};

class CharacterCounter {
public:
    std::vector<std::pair<int, char>> findMostFrequentCharacters(const std::map<char, int>& charCount, int n) {
        if (n <= 0) {
            logger->warn("N must be greater than 0. Received: {}", n);
            throw std::invalid_argument("N must be greater than 0");
        }

        std::vector<std::pair<int, char>> sortedChars(charCount.begin(), charCount.end());
        std::sort(sortedChars.begin(), sortedChars.end(), [](const auto& a, const auto& b) {
            return a.first > b.first;
            });

        sortedChars.resize(std::min(n, static_cast<int>(sortedChars.size())));
        logger->info("Retrieved {} most frequent characters", sortedChars.size());
        return sortedChars;
    }
};

TEST(CharacterCounterTest, CountCharactersInFile) {
    std::map<char, int> charCount = FileHandler::readCharacters("../data/data.txt");
    EXPECT_GT(charCount.size(), 0);
}

TEST(CharacterCounterTest, FindMostFrequentCharacters) {
    std::map<char, int> charCount = FileHandler::readCharacters("../data/data.txt");
    CharacterCounter counter;
    auto mostFrequentChars = counter.findMostFrequentCharacters(charCount, 3);
    EXPECT_EQ(mostFrequentChars.size(), 3);
}

TEST(CharacterCounterTest, EmptyFile) {
    std::ofstream out("../data/empty.txt");
    out.close();

    std::map<char, int> charCount = FileHandler::readCharacters("../data/empty.txt");
    EXPECT_EQ(charCount.size(), 0);
}

TEST(CharacterCounterTest, SingleCharacterFile) {
    std::ofstream out("../data/single.txt");
    out << "a";
    out.close();

    std::map<char, int> charCount = FileHandler::readCharacters("../data/single.txt");
    EXPECT_EQ(charCount.size(), 1);
    EXPECT_EQ(charCount['a'], 1);
}

TEST(CharacterCounterTest, MultipleDifferentCharacters) {
    std::ofstream out("../data/multiple.txt");
    out << "abcabc";
    out.close();

    std::map<char, int> charCount = FileHandler::readCharacters("../data/multiple.txt");
    EXPECT_EQ(charCount.size(), 3);
    EXPECT_EQ(charCount['a'], 2);
    EXPECT_EQ(charCount['b'], 2);
    EXPECT_EQ(charCount['c'], 2);
}

TEST(CharacterCounterTest, FileNotFound) {
    std::map<char, int> charCount = FileHandler::readCharacters("../data/nonexistent.txt");
    EXPECT_EQ(charCount.size(), 0);
}

TEST(CharacterCounterTest, FindMostFrequentCharactersZero) {
    std::map<char, int> charCount = FileHandler::readCharacters("../data/empty.txt");
    CharacterCounter counter;
    EXPECT_THROW(counter.findMostFrequentCharacters(charCount, 0), std::invalid_argument);
}

TEST(CharacterCounterTest, FindMostFrequentCharactersNegative) {
    std::map<char, int> charCount = FileHandler::readCharacters("../data/empty.txt");
    CharacterCounter counter;
    EXPECT_THROW(counter.findMostFrequentCharacters(charCount, -1), std::invalid_argument);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    spdlog::set_level(spdlog::level::info);

    return RUN_ALL_TESTS();
}