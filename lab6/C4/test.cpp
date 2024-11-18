#include "pch.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <gtest/gtest.h>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("console_logger");

class FileManager {
public:
     FileManager(const std::string& filePath) : filePath(filePath) {}

    std::string readFile() {
        std::ifstream inputFile(filePath);
        if (!inputFile) {
            logger->error("Error: File {} does not exist.", filePath);
            throw std::runtime_error("Error: File " + filePath + " does not exist.");
        }

        std::string content((std::istreambuf_iterator<char>(inputFile)),
            std::istreambuf_iterator<char>());

        if (content.empty()) {
            logger->error("Error: File {} is empty.", filePath);
            throw std::runtime_error("Error: File " + filePath + " is empty.");
        }

        logger->info("File {} successfully read.", filePath);
        return content;
    }

    void writeFile(const std::string& content, const std::string& outputPath) {
        std::ofstream outputFile(outputPath);
        if (!outputFile) {
            logger->error("Error: Unable to create file {}", outputPath);
            throw std::runtime_error("Error: Unable to create file " + outputPath);
        }
        outputFile << content;
        logger->info("Content written to file {}", outputPath);
    }

private:
    std::string filePath;
};

class TextProcessor {
public:
    static void convertWords(std::string& text) {
        std::istringstream stream(text);
        std::string word;
        std::ostringstream result;

        while (stream >> word) {
            if (word.length() > 2) {
                for (char& ch : word) {
                    ch = std::toupper(ch);
                }
            }
            result << word << ' ';
        }
        text = result.str();

        if (!text.empty()) {
            text.pop_back();
        }

        logger->info("Text processed: {}", text);
    }
};

class FileManagerTest : public ::testing::Test {
protected:
    const std::string dataDir = "data";
    const std::string testFilePath = "data/test.txt";
    const std::string outputFilePath = "data/output.txt";

    void SetUp() override {
        system("mkdir data");
        std::ofstream out(testFilePath);
        out << "Hello world from Google Test!";
        out.close();
        logger->info("Test file {} created.", testFilePath);
    }
};

TEST_F(FileManagerTest, ReadFile) {
    FileManager fileManager(testFilePath);
    EXPECT_NO_THROW({
        std::string content = fileManager.readFile();
        EXPECT_EQ(content, "Hello world from Google Test!");
        });
}

TEST_F(FileManagerTest, ReadEmptyFile) {
    std::remove(testFilePath.c_str());
    std::ofstream out(testFilePath);
    out.close();

    FileManager fileManager(testFilePath);
    EXPECT_THROW(fileManager.readFile(), std::runtime_error);
}

TEST_F(FileManagerTest, ReadNonExistentFile) {
    FileManager fileManager("data/non_existent.txt");
    EXPECT_THROW(fileManager.readFile(), std::runtime_error);
}

TEST_F(FileManagerTest, WriteFile) {
    FileManager fileManager(testFilePath);
    std::string content = "Test content";
    EXPECT_NO_THROW(fileManager.writeFile(content, outputFilePath));

    std::ifstream in(outputFilePath);
    std::string readContent((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_EQ(readContent, content);
}

TEST_F(FileManagerTest, WriteToExistingFile) {
    FileManager fileManager(testFilePath);
    std::string content = "New content";
    EXPECT_NO_THROW(fileManager.writeFile(content, testFilePath));

    std::ifstream in(testFilePath);
    std::string readContent((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    EXPECT_EQ(readContent, content);
}

TEST_F(FileManagerTest, WriteToInvalidPath) {
    FileManager fileManager(testFilePath);
    EXPECT_THROW(fileManager.writeFile("Test content", "data/invalid_dir/output.txt"), std::runtime_error);
}

TEST_F(FileManagerTest, ReadAfterWrite) {
    FileManager fileManager(testFilePath);
    std::string content = "Hello after write!";
    EXPECT_NO_THROW(fileManager.writeFile(content, testFilePath));

    EXPECT_NO_THROW({
        std::string readContent = fileManager.readFile();
        EXPECT_EQ(readContent, content);
        });
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}