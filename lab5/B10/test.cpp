#include "pch.h"

const std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("logger");

class FileManager {
private:
    std::string textFilePath;
    std::string wordListFilePath;

public:
    FileManager(const std::string& textFilePath, const std::string& wordListFilePath)
        : textFilePath(textFilePath), wordListFilePath(wordListFilePath) {}

    std::string readTextFile() {
        logger->info("Attempting to read text file: {}", textFilePath);
        std::ifstream file(textFilePath);
        if (!file.is_open()) {
            logger->error("Error opening text file: {}", textFilePath);
            throw std::runtime_error("Error opening text file: " + textFilePath);
        }

        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        if (content.empty()) {
            logger->error("The text file is empty: {}", textFilePath);
            throw std::runtime_error("The text file is empty: " + textFilePath);
        }

        logger->info("Successfully read text file: {}", textFilePath);
        return content;
    }

    std::vector<std::string> readWordList() {
        logger->info("Attempting to read word list file: {}", wordListFilePath);
        std::ifstream file(wordListFilePath);
        if (!file.is_open()) {
            logger->error("Error opening word list file: {}", wordListFilePath);
            throw std::runtime_error("Error opening word list file: " + wordListFilePath);
        }

        std::vector<std::string> words;
        std::string word;
        while (std::getline(file, word)) {
            words.push_back(word);
        }

        if (words.empty()) {
            logger->error("The word list file is empty: {}", wordListFilePath);
            throw std::runtime_error("The word list file is empty: " + wordListFilePath);
        }

        logger->info("Successfully read word list file: {}", wordListFilePath);
        return words;
    }

    std::string getTextFilePath() const {
        return textFilePath;
    }

    std::string getWordListFilePath() const {
        return wordListFilePath;
    }

    void setTextFilePath(const std::string& path) {
        textFilePath = path;
    }

    void setWordListFilePath(const std::string& path) {
        wordListFilePath = path;
    }
};

class WordCounter {
private:
    std::string text;

public:
    WordCounter(const std::string& text) : text(text) {}

    void countWordOccurrences(const std::vector<std::string>& words, std::map<std::string, int>& wordCount) const {
        logger->info("Counting word occurrences.");
        std::vector<std::string> sentences = splitSentences();
        for (const auto& sentence : sentences) {
            for (const auto& word : words) {
                int count = 0;
                std::string lowerSentence = toLower(sentence);
                std::string lowerWord = toLower(word);
                std::string::size_type pos = 0;

                while ((pos = lowerSentence.find(lowerWord, pos)) != std::string::npos) {
                    ++count;
                    pos += lowerWord.length();
                }
                wordCount[word] += count;
            }
        }
    }

    std::string getText() const {
        return text;
    }

    void setText(const std::string& newText) {
        text = newText;
    }

private:
    std::vector<std::string> splitSentences() const {
        std::vector<std::string> sentences;
        std::istringstream stream(text);
        std::string sentence;
        while (std::getline(stream, sentence, '.')) {
            if (!sentence.empty()) {
                sentences.push_back(sentence);
            }
        }
        return sentences;
    }

    static std::string toLower(const std::string& str) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return lowerStr;
    }
};

TEST(FileManagerTest, ReadTextFile) {
    FileManager fileManager("../data/test_text.txt", "../data/test_word_list.txt");

    std::ofstream outFile("../data/test_text.txt");
    outFile << "Hello world. This is a test.";
    outFile.close();

    std::string content = fileManager.readTextFile();
    EXPECT_EQ(content, "Hello world. This is a test.");
}

TEST(FileManagerTest, ReadWordList) {
    FileManager fileManager("../data/test_text.txt", "../data/test_word_list.txt");

    std::ofstream outFile("../data/test_word_list.txt");
    outFile << "Hello\nworld\ntest";
    outFile.close();

    std::vector<std::string> words = fileManager.readWordList();
    EXPECT_EQ(words.size(), 3);
    EXPECT_EQ(words[0], "Hello");
    EXPECT_EQ(words[1], "world");
    EXPECT_EQ(words[2], "test");
}

TEST(FileManagerTest, EmptyTextFile) {
    FileManager fileManager("../data/empty_text.txt", "../data/test_word_list.txt");

    std::ofstream outFile("../data/empty_text.txt");
    outFile.close();

    EXPECT_THROW(fileManager.readTextFile(), std::runtime_error);
}

TEST(FileManagerTest, EmptyWordListFile) {
    FileManager fileManager("../data/test_text.txt", "../data/empty_word_list.txt");

    std::ofstream outFile("../data/empty_word_list.txt");
    outFile.close();

    EXPECT_THROW(fileManager.readWordList(), std::runtime_error);
}

TEST(FileManagerTest, NonExistentTextFile) {
    FileManager fileManager("../data/non_existent.txt", "../data/test_word_list.txt");
    EXPECT_THROW(fileManager.readTextFile(), std::runtime_error);
}

TEST(FileManagerTest, NonExistentWordListFile) {
    FileManager fileManager("../data/test_text.txt", "../data/non_existent_word_list.txt");
    EXPECT_THROW(fileManager.readWordList(), std::runtime_error);
}

TEST(WordCounterTest, CountWordOccurrences) {
    WordCounter wordCounter("Hello world. Hello again. This is a test.");

    std::vector<std::string> words = { "Hello", "world", "test" };
    std::map<std::string, int> wordCount;

    wordCounter.countWordOccurrences(words, wordCount);

    EXPECT_EQ(wordCount["Hello"], 2);
    EXPECT_EQ(wordCount["world"], 1);
    EXPECT_EQ(wordCount["test"], 1);
}

TEST(WordCounterTest, CountWordOccurrencesEmptyText) {
    WordCounter wordCounter("");

    std::vector<std::string> words = { "Hello", "world", "test" };
    std::map<std::string, int> wordCount;

    wordCounter.countWordOccurrences(words, wordCount);

    EXPECT_EQ(wordCount["Hello"], 0);
    EXPECT_EQ(wordCount["world"], 0);
    EXPECT_EQ(wordCount["test"], 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    spdlog::set_level(spdlog::level::info);
    return RUN_ALL_TESTS();
}