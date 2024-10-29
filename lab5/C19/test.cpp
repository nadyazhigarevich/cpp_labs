#include "pch.h"

auto logger = spdlog::stdout_color_mt("console");

class Word {
private:
    std::string text;
    int length;
    int vowelCount;

    static int countVowels(const std::string& word) {
        int count = 0;
        for (char c : word) {
            char lower = std::tolower(c);
            if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u' || lower == 'y') {
                count++;
            }
        }
        return count;
    }

    friend class TextProcessor;

public:
    Word(const std::string& t) : text(t), length(t.size()), vowelCount(countVowels(t)) {
        logger->info("Word created: {}", text);
    }

    const std::string& getText() const {
        return text;
    }
};

class FileManager {
public:
    static void readFromFile(const std::string& filename, std::vector<Word>& words) {
        logger->info("Attempting to read from file: {}", filename);
        std::ifstream inputFile(filename);
        if (!inputFile) {
            logger->error("Failed to open file: {}", filename);
            logger->flush();
            throw std::runtime_error("Failed to open file.");
        }

        if (inputFile.peek() == std::ifstream::traits_type::eof()) {
            logger->warn("File is empty: {}", filename);
            logger->flush();
            throw std::runtime_error("File is empty.");
        }

        std::string word;
        while (inputFile >> word) {
            word.erase(std::remove_if(word.begin(), word.end(), ::ispunct), word.end());
            if (!word.empty()) {
                words.emplace_back(word);
                logger->info("Added word: {}", word);
            }
        }
    }
};

class TextProcessor {
private:
    std::vector<Word> words;

public:
    void sortWords() {
        logger->info("Sorting words...");
        std::sort(words.begin(), words.end(), [](const Word& a, const Word& b) {
            if (a.length != b.length) {
                return a.length > b.length;
            }
            return a.vowelCount < b.vowelCount;
            });
        logger->info("Sorting complete.");
    }

    void printWords() const {
        logger->info("Printing words...");
        for (const auto& w : words) {
            std::cout << w.getText() << std::endl;
        }
        logger->info("Finished printing words.");
    }

    std::vector<Word>& getWords() {
        return words;
    }
};

class TextProcessorTest : public ::testing::Test {
protected:
    TextProcessor processor;

    void SetUp() override {
        processor = TextProcessor();
    }
};

TEST_F(TextProcessorTest, AddWords) {
    processor.getWords().emplace_back(Word("hello"));
    processor.getWords().emplace_back(Word("world"));

    ASSERT_EQ(processor.getWords().size(), 2);
    EXPECT_EQ(processor.getWords()[0].getText(), "hello");
    EXPECT_EQ(processor.getWords()[1].getText(), "world");
}

TEST_F(TextProcessorTest, SortWords) {
    processor.getWords().emplace_back(Word("apple"));
    processor.getWords().emplace_back(Word("banana"));
    processor.getWords().emplace_back(Word("kiwi"));

    processor.sortWords();

    EXPECT_EQ(processor.getWords()[0].getText(), "banana");
    EXPECT_EQ(processor.getWords()[1].getText(), "apple");
    EXPECT_EQ(processor.getWords()[2].getText(), "kiwi");
}

TEST(FileManagerTest, EmptyFile) {
    std::ofstream outFile("../data/empty.txt");
    outFile.close();

    std::vector<Word> words;
    EXPECT_THROW(FileManager::readFromFile("../data/empty.txt", words), std::runtime_error);
}

TEST(FileManagerTest, NonExistentFile) {
    std::vector<Word> words;
    EXPECT_THROW(FileManager::readFromFile("../data/nonexistent.txt", words), std::runtime_error);
}

TEST_F(TextProcessorTest, SortNoWords) {
    processor.sortWords();
    EXPECT_EQ(processor.getWords().size(), 0);
}

TEST_F(TextProcessorTest, PrintNoWords) {
    testing::internal::CaptureStdout();
    processor.printWords();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "");
}

TEST_F(TextProcessorTest, AddPunctuationOnly) {
    processor.getWords().emplace_back(Word("..."));
    EXPECT_EQ(processor.getWords().size(), 1);
    EXPECT_EQ(processor.getWords()[0].getText(), "...");
}

TEST_F(TextProcessorTest, AddWordWithSpaces) {
    processor.getWords().emplace_back(Word("   "));
    EXPECT_EQ(processor.getWords().size(), 1);
    EXPECT_EQ(processor.getWords()[0].getText(), "   ");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}