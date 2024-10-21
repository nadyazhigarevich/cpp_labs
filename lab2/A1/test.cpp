#include "pch.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <chrono>


using namespace std;

class Word {
private:
    string value;

public:
    Word(const string& value) : value(value) {}


    void setValue(const string& newValue) {
        value = newValue;
    }

    string toString() const {
        return value;
    }

    bool equals(const Word& other) const { 
        return value == other.value;
    }

    size_t hashCode() const {
        return hash<string>()(value);
    }
};

class Sentence {
private:
    vector<Word> words;

public:
    void addWord(const Word& word) { 
        words.push_back(word);
    }

    string toString() const {
        string result;
        for (const auto& word : words) {
            result += word.toString() + " ";
        }
        return result.empty() ? result : result.substr(0, result.size() - 1);
    }

    bool equals(const Sentence& other) const {
        if (words.size() != other.words.size()) return false;
        for (size_t i = 0; i < words.size(); ++i) {
            if (!words[i].equals(other.words[i])) return false;
        }
        return true;
    }

    size_t hashCode() const {
        size_t hash = 0;
        for (const auto& word : words) {
            hash ^= word.hashCode(); 
        }
        return hash;
    }

    const vector<Word>& getWords() const {
        return words;
    }

    void setWords(const vector<Word>& newWords) {
        words = newWords;
    }
};

class SentenceService {
public:
    static string toString(const Sentence& sentence) {
        return sentence.toString();
    }

    static bool equals(const Sentence& lhs, const Sentence& rhs) {
        return lhs.equals(rhs);
    }

    static size_t hashCode(const Sentence& sentence) {
        return sentence.hashCode();
    }
};

class TextService {
public:
    static string printText(const string& title, const vector<Sentence>& sentences) { 
        ostringstream oss;
        oss << "Title: " << title << "\n";
        for (const auto& sentence : sentences) {
            oss << SentenceService::toString(sentence) << "\n";
        }
        return oss.str();
    }
};

class Text {
private:
    vector<Sentence> sentences;
    string title;

public:
    Text(const string& title) : title(title) {}

    void addSentence(const Sentence& sentence) {
        sentences.push_back(sentence);
    }

    string getTitle() const {
        return title;
    }

    void setTitle(const string& newTitle) {
        title = newTitle;
    }

    string toString() const {
        return TextService::printText(title, sentences);
    }

    const vector<Sentence>& getSentences() const {
        return sentences;
    }

    void setSentences(const vector<Sentence>& newSentences) {
        sentences = newSentences;
    }
};

#include <gtest/gtest.h> 

class WordTest : public ::testing::Test { 
public:
    Word word1{ "Hello" };
    Word word2{ "World" };
    Word word3{ "Hello" };
};

TEST_F(WordTest, GetValue) {
    string expected = "Hello";
    string actual = word1.toString();
    EXPECT_EQ(actual, expected);
}

TEST_F(WordTest, SetValue) {
    word2.setValue("NewWorld");
    string expected = "NewWorld";
    string actual = word2.toString();
    EXPECT_EQ(actual, expected);
}

TEST_F(WordTest, ToString) {
    string expected1 = "Hello";
    string expected2 = "World";
    string actual1 = word1.toString();
    string actual2 = word2.toString();
    EXPECT_EQ(actual1, expected1);
    EXPECT_EQ(actual2, expected2);
}

TEST_F(WordTest, Equals_SameValue) {
    bool expected = true;
    bool actual = word1.equals(word3);
    EXPECT_EQ(actual, expected);
}

TEST_F(WordTest, HashCode) {
    size_t expected1 = word3.hashCode();
    size_t expected2 = word2.hashCode();
    size_t actual1 = word1.hashCode();
    EXPECT_EQ(actual1, expected1);
    EXPECT_NE(actual1, expected2);
}

TEST_F(WordTest, SetValue_EmptyValue) {
    word2.setValue("");
    string expected = "";
    string actual = word2.toString();
    EXPECT_EQ(actual, expected);
}

TEST_F(WordTest, ToString_EmptyWord) {
    Word emptyWord("");
    string expected = "";
    string actual = emptyWord.toString();
    EXPECT_EQ(actual, expected);
}

TEST_F(WordTest, Equals_DifferentValue) {
    bool expected1 = false;
    Word emptyWord("");
    bool actual1 = word1.equals(word2);
    bool actual2 = word1.equals(emptyWord);
    EXPECT_EQ(actual1, expected1);
    EXPECT_EQ(actual2, expected1);
}

TEST_F(WordTest, HashCode_EmptyWord) {
    Word emptyWord("");
    size_t expected = word1.hashCode();
    size_t actual = emptyWord.hashCode();
    EXPECT_NE(actual, expected);
}

class WordParamTest : public ::testing::TestWithParam<tuple<string, size_t>> {}; // ?

TEST_P(WordParamTest, HashCodeConsistency) {
    string wordValue;
    size_t expectedHash;
    tie(wordValue, expectedHash) = GetParam();

    Word word(wordValue);
    size_t actualHash = word.hashCode();
    EXPECT_EQ(actualHash, expectedHash);
}

INSTANTIATE_TEST_CASE_P(
    HashCodeTestSuite,
    WordParamTest,
    ::testing::Values(
        make_tuple("Hello", hash<string>()("Hello")),
        make_tuple("World", hash<string>()("World")),
        make_tuple("", hash<string>()("")),
        make_tuple("Test", hash<string>()("Test"))
    )
); 

class SentenceTest : public ::testing::Test {
public:
    Sentence sentence;
    Word word1{ "Hello" };
    Word word2{ "World" };
    Word word3{ "Test" };
    Word word4{ "Hello" };
};

TEST_F(SentenceTest, AddWord) {
    sentence.addWord(word1);
    string expected = "Hello";
    string actual = sentence.toString();
    EXPECT_EQ(actual, expected);

    sentence.addWord(word2);
    expected = "Hello World";
    actual = sentence.toString();
    EXPECT_EQ(actual, expected);
}

TEST_F(SentenceTest, ToString_EmptySentence) {
    string expected = "";
    string actual = sentence.toString();
    EXPECT_EQ(actual, expected);
}

TEST_F(SentenceTest, ToString_WithWords) {
    sentence.addWord(word1);
    sentence.addWord(word2);
    string expected = "Hello World";
    string actual = sentence.toString();
    EXPECT_EQ(actual, expected);

    sentence.addWord(word3);
    expected = "Hello World Test";
    actual = sentence.toString();
    EXPECT_EQ(actual, expected);
}

TEST_F(SentenceTest, Equals_SameSentences) {
    Sentence otherSentence;
    otherSentence.addWord(word1);
    otherSentence.addWord(word2);

    bool expected = true;
    bool actual = SentenceService::equals(sentence, otherSentence);
    EXPECT_NE(actual, expected);
}

TEST_F(SentenceTest, Equals_DifferentSentences) {
    Sentence otherSentence;
    otherSentence.addWord(word1);
    otherSentence.addWord(word3);

    bool expected = false;
    bool actual = SentenceService::equals(sentence, otherSentence);
    EXPECT_EQ(actual, expected);
}

TEST_F(SentenceTest, HashCode_Consistency) {
    sentence.addWord(word1);
    sentence.addWord(word2);

    size_t hash1 = SentenceService::hashCode(sentence);
    size_t hash2 = SentenceService::hashCode(sentence);
    EXPECT_EQ(hash1, hash2);
}

TEST_F(SentenceTest, HashCode_DifferentSentences) {
    Sentence otherSentence;
    otherSentence.addWord(word1);
    otherSentence.addWord(word3);

    size_t hash1 = SentenceService::hashCode(sentence);
    size_t hash2 = SentenceService::hashCode(otherSentence);
    EXPECT_NE(hash1, hash2);
}

TEST_F(SentenceTest, SetWords) {
    vector<Word> newWords = { word1, word2, word3 };
    sentence.setWords(newWords);
    string expected = "Hello World Test";
    string actual = sentence.toString();
    EXPECT_EQ(actual, expected);
}

TEST_F(SentenceTest, GetWords) {
    sentence.addWord(word1);
    sentence.addWord(word2);

    const auto& words = sentence.getWords();
    size_t expectedSize = 2;
    EXPECT_EQ(words.size(), expectedSize);
    EXPECT_EQ(words[0].toString(), "Hello");
    EXPECT_EQ(words[1].toString(), "World");
}

TEST_F(SentenceTest, Equals_DifferentSizes) {
    Sentence otherSentence;
    otherSentence.addWord(word1);

    bool expected = false;
    bool actual = SentenceService::equals(sentence, otherSentence);
    EXPECT_EQ(actual, expected);
}

TEST_F(SentenceTest, HashCode_EmptySentence) {
    Sentence emptySentence;
    EXPECT_EQ(SentenceService::hashCode(sentence), SentenceService::hashCode(emptySentence));
}

class SentenceParamTest : public ::testing::TestWithParam<tuple<vector<Word>, string>> {}; //?

TEST_P(SentenceParamTest, ToString_WithParam) {
    vector<Word> words;
    string expectedString;
    tie(words, expectedString) = GetParam(); // tie?

    Sentence s;
    for (const auto& word : words) {
        s.addWord(word);
    }

    string actualString = s.toString();
    EXPECT_EQ(actualString, expectedString);
}

INSTANTIATE_TEST_CASE_P(
    SentenceTestSuite,
    SentenceParamTest,
    ::testing::Values(
        make_tuple(vector<Word>{Word{ "Hello" }, Word{ "World" }}, "Hello World"),
        make_tuple(vector<Word>{Word{ "This" }, Word{ "is" }, Word{ "a" }, Word{ "test" }}, "This is a test"),
        make_tuple(vector<Word>{}, ""),
        make_tuple(vector<Word>{Word{ "SingleWord" }}, "SingleWord")
    )
);

class TextTest : public ::testing::Test {
public:
    Text text{ "Sample Title" };
    Sentence sentence1;
    Sentence sentence2;

    void SetUp() override {
        sentence1.addWord(Word{ "Hello" });
        sentence1.addWord(Word{ "world" });
        sentence2.addWord(Word{ "This" });
        sentence2.addWord(Word{ "is" });
        sentence2.addWord(Word{ "a" });
        sentence2.addWord(Word{ "test" });
    }
};

TEST_F(TextTest, GetTitle) {
    string expected = "Sample Title";
    string actual = text.getTitle();
    EXPECT_EQ(actual, expected);
}

TEST_F(TextTest, SetTitle) {
    text.setTitle("New Title");
    string expected = "New Title";
    string actual = text.getTitle();
    EXPECT_EQ(actual, expected);
}

TEST_F(TextTest, AddSentence) {
    text.addSentence(sentence1);
    size_t expectedSize = 1;
    size_t actualSize = text.getSentences().size();
    EXPECT_EQ(actualSize, expectedSize);
    string expectedSentence = "Hello world";
    string actualSentence = text.getSentences()[0].toString();
    EXPECT_EQ(actualSentence, expectedSentence);

    text.addSentence(sentence2);
    expectedSize = 2;
    actualSize = text.getSentences().size();
    EXPECT_EQ(actualSize, expectedSize);
    expectedSentence = "This is a test";
    actualSentence = text.getSentences()[1].toString();
    EXPECT_EQ(actualSentence, expectedSentence);
}

TEST_F(TextTest, PrintText) {
    ostringstream oss;
    streambuf* oldCout = cout.rdbuf(oss.rdbuf());

    text.addSentence(sentence1);
    text.addSentence(sentence2);
    string actualOutput = text.toString();

    string expectedOutput = "Title: Sample Title\nHello world\nThis is a test\n";
    EXPECT_EQ(actualOutput, expectedOutput);
}

TEST_F(TextTest, ToString) {
    text.addSentence(sentence1);
    text.addSentence(sentence2);

    string expectedOutput = "Title: Sample Title\nHello world\nThis is a test\n";
    string actualOutput = text.toString();
    EXPECT_EQ(actualOutput, expectedOutput);
}

TEST_F(TextTest, SetTitle_EmptyString) {
    text.setTitle("");
    string expected = "";
    string actual = text.getTitle();
    EXPECT_EQ(actual, expected);
}

TEST_F(TextTest, GetSentences_InitiallyEmpty) {
    EXPECT_TRUE(text.getSentences().empty());
}

class TextParamTest : public ::testing::TestWithParam<tuple<string, vector<Sentence>, string>> {};

TEST_P(TextParamTest, ToString_WithParam) {
    string title;
    vector<Sentence> sentences;
    string expectedString;
    tie(title, sentences, expectedString) = GetParam();

    Text t(title);
    for (const auto& sentence : sentences) {
        t.addSentence(sentence);
    }

    string actualString = t.toString();
    EXPECT_EQ(actualString, expectedString);
}

INSTANTIATE_TEST_CASE_P(
    TextTestSuite,
    TextParamTest,
    ::testing::Values(
        make_tuple("Title 1", vector<Sentence>{Sentence{}}, "Title: Title 1\n\n"),
        make_tuple("Title 2", vector<Sentence>{Sentence{}, Sentence{}}, "Title: Title 2\n\n\n"),
        make_tuple("Title 3", vector<Sentence>{Sentence{}}, "Title: Title 3\n\n")
    )
);

TEST_F(TextTest, Performance_AddSentences) {
    const int numSentences = 1000;
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < numSentences; ++i) {
        Sentence sentence;
        sentence.addWord(Word{ "Sentence" });
        sentence.addWord(Word{ to_string(i) });
        text.addSentence(sentence);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;

    size_t expectedSize = numSentences;
    size_t actualSize = text.getSentences().size();
    EXPECT_EQ(actualSize, expectedSize);
    EXPECT_LT(duration.count(), 1.0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}