#include "pch.h"

std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("console");

class InputStream {
public:
    virtual std::string readLine() = 0;
    virtual ~InputStream() = default;
};

class OutputStream {
public:
    virtual void print(const std::string& str) = 0;
    virtual void println(const std::string& str) {
        print(str + "\n");
    }
    virtual ~OutputStream() = default;
};

class FileInputStream : public InputStream {
private:
    std::ifstream file;

public:
    FileInputStream(const std::string& filename) {
        std::string fullPath = "../data/" + filename;
        file.open(fullPath);
        if (!file.is_open()) {
            logger->error("Failed to open file: {}", fullPath);
            throw std::runtime_error("Failed to open file: " + fullPath);
        }

        file.seekg(0, std::ios::end);
        if (file.tellg() == 0) {
            logger->error("File is empty: {}", fullPath);
            throw std::runtime_error("File is empty: " + fullPath);
        }
        file.seekg(0, std::ios::beg);
    }

    std::string readLine() override {
        std::string line;
        if (std::getline(file, line)) {
            return line;
        }
        return "";
    }

    ~FileInputStream() {
        if (file.is_open()) {
            file.close();
        }
    }
};

class FileOutputStream : public OutputStream {
private:
    std::ofstream file;

public:
    FileOutputStream(const std::string& filename) {
        std::string fullPath = "../data/" + filename;
        file.open(fullPath);
        if (!file.is_open()) {
            logger->error("Failed to open file: {}", fullPath);
            throw std::runtime_error("Failed to open file: " + fullPath);
        }
    }

    void print(const std::string& str) override {
        file << str;
        logger->info("Printed to file: {}", str);
    }

    ~FileOutputStream() {
        if (file.is_open()) {
            file.close();
        }
    }
};

class OStringStreamOutputStream : public OutputStream {
private:
    std::ostringstream& oss;

public:
    OStringStreamOutputStream(std::ostringstream& o) : oss(o) {}

    void print(const std::string& str) override {
        oss << str;
        logger->info("Printed to string stream: {}", str);
    }
};

class VowelWordFinder {
private:
    OutputStream& outputStream;

    bool startsWithVowel(const std::string& word) {
        if (word.empty()) return false;
        char first = std::tolower(word[0]);
        return first == 'a' || first == 'e' || first == 'i' || first == 'o' || first == 'u';
    }

    void findVowelWords(const std::string& line) {
        std::istringstream stream(line);
        std::string word;
        std::vector<std::string> vowelWords;

        while (stream >> word) {
            if (startsWithVowel(word)) {
                vowelWords.push_back(word);
            }
        }

        if (!vowelWords.empty()) {
            outputStream.print("Words starting with a vowel: ");
            for (const auto& vw : vowelWords) {
                outputStream.print(vw + " ");
            }
            outputStream.println("");
            logger->info("Found vowel words: {}", fmt::join(vowelWords, ", "));
        }
        else {
            outputStream.println("No words starting with a vowel.");
            logger->info("No words starting with a vowel in line: {}", line);
        }
    }

public:
    VowelWordFinder(OutputStream& os) : outputStream(os) {}

    void processLine(const std::string& line) {
        if (line.empty()) {
            outputStream.println("Empty line skipped.");
            logger->info("Skipped empty line.");
            return;
        }
        findVowelWords(line);
    }
};

class FileCreator {
public:
    static void createEmptyFile(const std::string& filename) {
        std::ofstream outFile("../data/" + filename);
        if (!outFile) {
            logger->error("Error creating file: {}", filename);
            std::cerr << "Error creating file: " << filename << std::endl;
        }
        logger->info("Created empty file: {}", filename);
    }
};

class TestRunner {
public:
    static void runTests(int argc, char** argv) {
        ::testing::InitGoogleTest(&argc, argv);
        RUN_ALL_TESTS();
    }
};

TEST(FileInputStreamTests, ReadLine) {
    const std::string testFile = "test.txt";
    {
        std::ofstream outFile("../data/" + testFile);
        outFile << "Hello World\nAnother line\n";
        outFile.close();
    }

    FileInputStream inputStream(testFile);
    EXPECT_EQ(inputStream.readLine(), "Hello World");
    EXPECT_EQ(inputStream.readLine(), "Another line");
    EXPECT_EQ(inputStream.readLine(), "");
}

TEST(FileInputStreamTests, EmptyFile) {
    const std::string testFile = "empty.txt";
    FileCreator::createEmptyFile(testFile);
    std::cout << "Empty file created: " << testFile << std::endl;

    EXPECT_THROW({
        FileInputStream inputStream(testFile);
        }, std::runtime_error);
}

TEST(FileInputStreamTests, NonExistentFile) {
    const std::string testFile = "non_existent.txt";
    EXPECT_THROW({
        FileInputStream inputStream(testFile);
        }, std::runtime_error);
}

TEST(FileOutputStreamTests, PrintLine) {
    const std::string outputFile = "output.txt";
    {
        FileOutputStream outputStream(outputFile);
        outputStream.println("Hello World");
        outputStream.println("Another line");
    }

    std::ifstream inFile("../data/" + outputFile);
    std::string line;
    std::getline(inFile, line);
    EXPECT_EQ(line, "Hello World");
    std::getline(inFile, line);
    EXPECT_EQ(line, "Another line");
}

TEST(VowelWordFinderTests, ProcessLineWithVowels) {
    std::ostringstream oss;
    OStringStreamOutputStream outputStream(oss);
    VowelWordFinder finder(outputStream);

    finder.processLine("Apple banana orange");
    EXPECT_NE(oss.str().find("Words starting with a vowel:"), std::string::npos);
}

TEST(VowelWordFinderTests, ProcessLineWithNoVowels) {
    std::ostringstream oss;
    OStringStreamOutputStream outputStream(oss);
    VowelWordFinder finder(outputStream);

    finder.processLine("No vowels here");
    EXPECT_NE(oss.str().find("No words starting with a vowel."), std::string::npos);
}

TEST(VowelWordFinderTests, ProcessLineWithMixedCase) {
    std::ostringstream oss;
    OStringStreamOutputStream outputStream(oss);
    VowelWordFinder finder(outputStream);

    finder.processLine("apple Banana Orange");
    EXPECT_NE(oss.str().find("Words starting with a vowel:"), std::string::npos);
}

TEST(VowelWordFinderTests, ProcessEmptyLine) {
    std::ostringstream oss;
    OStringStreamOutputStream outputStream(oss);
    VowelWordFinder finder(outputStream);

    finder.processLine("");
    EXPECT_NE(oss.str().find("Empty line skipped."), std::string::npos);
}

int main(int argc, char** argv) {
    TestRunner::runTests(argc, argv);
    return 0;
}