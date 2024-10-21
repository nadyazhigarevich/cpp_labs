#include "pch.h"

#include <string>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

class Tariff {
private:
    string name;
    double monthlyFee;
    int clientCount;

public:
    Tariff(const string& name, double monthlyFee)
        : name(name), monthlyFee(monthlyFee), clientCount(0) {
        if (monthlyFee < 0) {
            throw invalid_argument("Monthly fee cannot be negative");
        }
    }

    string getName() const { return name; }
    double getMonthlyFee() const { return monthlyFee; }
    int getClientCount() const { return clientCount; }
    void incrementClientCount() { clientCount++; }

    virtual string toString() const = 0;
};

class PrepaidTariff : public Tariff {
private:
    double callRate;

public:
    PrepaidTariff(const string& name, double monthlyFee, double callRate)
        : Tariff(name, monthlyFee), callRate(callRate) {}

    double getCallRate() const { return callRate; }

    string toString() const override {
        return "Prepaid Tariff: " + getName() +
            ", Monthly Fee: " + to_string(getMonthlyFee()) +
            ", Call Rate: " + to_string(callRate) + "\n";
    }
};

class PostpaidTariff : public Tariff {
private:
    double includedMinutes;

public:
    PostpaidTariff(const string& name, double monthlyFee, double includedMinutes)
        : Tariff(name, monthlyFee), includedMinutes(includedMinutes) {}

    double getIncludedMinutes() const { return includedMinutes; }

    string toString() const override {
        return "Postpaid Tariff: " + getName() +
            ", Monthly Fee: " + to_string(getMonthlyFee()) +
            ", Included Minutes: " + to_string(includedMinutes) + "\n";
    }
};

class TariffService {
private:
    vector<Tariff*> tariffs;

public:
    TariffService() {
        tariffs.push_back(new PrepaidTariff("Prepaid Plan A", 10.0, 0.5));
        tariffs.push_back(new PostpaidTariff("Postpaid Plan B", 20.0, 100));
        tariffs.push_back(new PrepaidTariff("Prepaid Plan C", 15.0, 0.4));
        tariffs.push_back(new PostpaidTariff("Postpaid Plan D", 25.0, 200));
    }

    ~TariffService() {
        for (auto tariff : tariffs) {
            delete tariff;
        }
    }

    void addTariff(Tariff* tariff) {
        tariffs.push_back(tariff);
    }

    int calculateTotalClients() const {
        int total = 0;
        for (const auto& tariff : tariffs) {
            total += tariff->getClientCount();
        }
        return total;
    }

    void sortTariffsByMonthlyFee() {
        sort(tariffs.begin(), tariffs.end(), [](Tariff* a, Tariff* b) {
            return a->getMonthlyFee() < b->getMonthlyFee();
            });
    }

    vector<Tariff*> findTariffsWithinRange(double min, double max) const {
        vector<Tariff*> result;
        for (const auto& tariff : tariffs) {
            if (tariff->getMonthlyFee() >= min && tariff->getMonthlyFee() <= max) {
                result.push_back(tariff);
            }
        }
        return result;
    }

};

class TestTariff : public Tariff {
public:
    TestTariff(const string& name, double monthlyFee)
        : Tariff(name, monthlyFee) {}

    string  toString() const override {
        return "Test Tariff: " + getName() +
            ", Monthly Fee: " + to_string(getMonthlyFee()) +
            ", Clients: " + to_string(getClientCount()) + "\n";
    }
};

TEST(TariffTest, Initialization) {
    TestTariff tariff("Basic Plan", 15.99);

    string expectedName = "Basic Plan";
    double expectedMonthlyFee = 15.99;
    int expectedClientCount = 0;

    string actualName = tariff.getName();
    double actualMonthlyFee = tariff.getMonthlyFee();
    int actualClientCount = tariff.getClientCount();

    EXPECT_EQ(actualName, expectedName);
    EXPECT_EQ(actualMonthlyFee, expectedMonthlyFee);
    EXPECT_EQ(actualClientCount, expectedClientCount);
}

TEST(TariffTest, IncrementClientCount) {
    TestTariff tariff("Basic Plan", 15.99);
    tariff.incrementClientCount();

    int expectedCountAfterFirstIncrement = 1;
    int actualCountAfterFirstIncrement = tariff.getClientCount();
    EXPECT_EQ(actualCountAfterFirstIncrement, expectedCountAfterFirstIncrement);
}

TEST(TariffTest, TariffInfo) {
    TestTariff tariff("Basic Plan", 15.99);

    string expectedInfo = "Test Tariff: Basic Plan, Monthly Fee: 15.990000, Clients: 0\n";
    string actualInfo = tariff.toString();

    EXPECT_EQ(actualInfo, expectedInfo);
}

TEST(TariffTest, NegativeMonthlyFee) {
    EXPECT_THROW(TestTariff tariff("Invalid Plan", -5.0), invalid_argument);
}

class TariffParamTest : public ::testing::TestWithParam<tuple<string, double, int>> {};

TEST_P(TariffParamTest, ParametrizedTest) {
    string name = get<0>(GetParam());
    double monthlyFee = get<1>(GetParam());
    int expectedClients = get<2>(GetParam());

    TestTariff tariff(name, monthlyFee);

    string actualName = tariff.getName();
    double actualMonthlyFee = tariff.getMonthlyFee();
    int actualClients = tariff.getClientCount();

    EXPECT_EQ(actualName, name);
    EXPECT_EQ(actualMonthlyFee, monthlyFee);
    EXPECT_EQ(actualClients, expectedClients);
}

INSTANTIATE_TEST_CASE_P(
    TariffTests,
    TariffParamTest,
    ::testing::Values(
        make_tuple("Plan A", 10.0, 0),
        make_tuple("Plan B", 20.0, 0),
        make_tuple("Plan C", 30.0, 0)
    )
);

class PrepaidTariffTest : public ::testing::Test {};

TEST_F(PrepaidTariffTest, Initialization) {
    PrepaidTariff tariff("Prepaid Plan A", 10.0, 0.5);

    string expectedName = "Prepaid Plan A";
    double expectedMonthlyFee = 10.0;
    int expectedClientCount = 0;

    string actualName = tariff.getName();
    double actualMonthlyFee = tariff.getMonthlyFee();
    int actualClientCount = tariff.getClientCount();

    EXPECT_EQ(actualName, expectedName);
    EXPECT_EQ(actualMonthlyFee, expectedMonthlyFee);
    EXPECT_EQ(actualClientCount, expectedClientCount);
}

TEST_F(PrepaidTariffTest, CallRate) {
    PrepaidTariff tariff("Prepaid Plan A", 10.0, 0.5);
    string info = tariff.toString();

    string expectedInfo = "Prepaid Tariff: Prepaid Plan A, Monthly Fee: 10.000000, Call Rate: 0.500000\n";
    string actualInfo = info;

    EXPECT_EQ(actualInfo, expectedInfo);
}

TEST_F(PrepaidTariffTest, NegativeMonthlyFee) {
    EXPECT_THROW(PrepaidTariff tariff("Invalid Plan", -10.0, 0.5), invalid_argument);
}

TEST_F(PrepaidTariffTest, NegativeCallRate) {
    EXPECT_NO_THROW(PrepaidTariff tariff("Valid Plan", 10.0, -0.5));
}

class PrepaidTariffParamTest : public ::testing::TestWithParam<tuple<string, double, double, bool>> {};

TEST_P(PrepaidTariffParamTest, ParametrizedTest) {
    string name = get<0>(GetParam());
    double monthlyFee = get<1>(GetParam());
    double callRate = get<2>(GetParam());
    bool shouldThrow = get<3>(GetParam());

    if (shouldThrow) {
        EXPECT_THROW(PrepaidTariff tariff(name, monthlyFee, callRate), invalid_argument);
    }
    else {
        EXPECT_NO_THROW(PrepaidTariff tariff(name, monthlyFee, callRate));
    }
}

INSTANTIATE_TEST_CASE_P(
    PrepaidTariffTests,
    PrepaidTariffParamTest,
    ::testing::Values(
        make_tuple("Plan A", -10.0, 0.5, true),
        make_tuple("Plan B", 10.0, -0.5, false),
        make_tuple("Plan C", -5.0, -0.5, true)
    )
);

class PostpaidTariffTest : public ::testing::Test {};

TEST_F(PostpaidTariffTest, Initialization) {
    PostpaidTariff tariff("Postpaid Plan A", 20.0, 100);

    string expectedName = "Postpaid Plan A";
    double expectedMonthlyFee = 20.0;
    int expectedClientCount = 0;

    string actualName = tariff.getName();
    double actualMonthlyFee = tariff.getMonthlyFee();
    int actualClientCount = tariff.getClientCount();

    EXPECT_EQ(actualName, expectedName);
    EXPECT_EQ(actualMonthlyFee, expectedMonthlyFee);
    EXPECT_EQ(actualClientCount, expectedClientCount);
}

TEST_F(PostpaidTariffTest, TariffInfo) {
    PostpaidTariff tariff("Postpaid Plan A", 20.0, 100);
    string info = tariff.toString();

    string expectedInfo = "Postpaid Tariff: Postpaid Plan A, Monthly Fee: 20.000000, Included Minutes: 100.000000\n";
    string actualInfo = info;

    EXPECT_EQ(actualInfo, expectedInfo);
}

TEST_F(PostpaidTariffTest, NegativeMonthlyFee) {
    EXPECT_THROW(PostpaidTariff tariff("Invalid Plan", -20.0, 100), invalid_argument);
}

TEST_F(PostpaidTariffTest, NegativeIncludedMinutes) {
    EXPECT_NO_THROW(PostpaidTariff tariff("Valid Plan", 20.0, -50));
}

class PostpaidTariffParamTest : public ::testing::TestWithParam<tuple<string, double, double, bool>> {};

TEST_P(PostpaidTariffParamTest, ParametrizedTest) {
    string name = get<0>(GetParam());
    double monthlyFee = get<1>(GetParam());
    double includedMinutes = get<2>(GetParam());
    bool shouldThrow = get<3>(GetParam());

    if (shouldThrow) {
        EXPECT_THROW(PostpaidTariff tariff(name, monthlyFee, includedMinutes), invalid_argument);
    }
    else {
        EXPECT_NO_THROW(PostpaidTariff tariff(name, monthlyFee, includedMinutes));
    }
}

INSTANTIATE_TEST_CASE_P(
    PostpaidTariffTests,
    PostpaidTariffParamTest,
    ::testing::Values(
        make_tuple("Plan A", -20.0, 100, true),
        make_tuple("Plan B", 20.0, -50, false),
        make_tuple("Plan C", -10.0, -30, true)
    )
);

TEST(TariffService, CalculateTotalClients) {
    TariffService service;

    int expectedTotalClients = 0;
    int actualTotalClients = service.calculateTotalClients();

    EXPECT_EQ(actualTotalClients, expectedTotalClients);
}

TEST(TariffService, AddTariff) {
    TariffService service;
    PrepaidTariff* newTariff = new PrepaidTariff("New Prepaid Plan", 12.0, 0.6);
    service.addTariff(newTariff);

    int expectedTotalClients = 0;
    int actualTotalClients = service.calculateTotalClients();

    EXPECT_EQ(actualTotalClients, expectedTotalClients);
}

TEST(TariffService, AddNullTariff) {
    TariffService service;
    EXPECT_NO_THROW(service.addTariff(nullptr));
}

TEST(TariffService, AddTariffPerformance) {
    TariffService service;

    auto start = chrono::high_resolution_clock::now();
    service.addTariff(new PrepaidTariff("New Prepaid Plan", 30.0, 0.6));
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> duration = end - start;
    EXPECT_LT(duration.count(), 1000.0);
}

TEST(TariffService, SortTariffsPerformance) {
    TariffService service;

    auto start = chrono::high_resolution_clock::now();
    service.sortTariffsByMonthlyFee();
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> duration = end - start;
    EXPECT_LT(duration.count(), 1000.0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}