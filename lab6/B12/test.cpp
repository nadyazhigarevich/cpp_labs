#include "pch.h"

using namespace std;

auto logger = spdlog::stdout_color_mt("tariff_logger");

class Tariff {
private:
    string name;
    double monthlyFee;
    int clientCount;

public:
    Tariff(const string& name, double monthlyFee)
        : name(name), monthlyFee(monthlyFee), clientCount(0) {
        if (monthlyFee < 0) {
            logger->error("Monthly fee cannot be negative: {}", monthlyFee);
            throw invalid_argument("Monthly fee cannot be negative");
        }
    }

    string getName() const { return name; }
    double getMonthlyFee() const { return monthlyFee; }
    int getClientCount() const { return clientCount; }
    void incrementClientCount() { clientCount++; }

    void setMonthlyFee(double fee) {
        if (fee < 0) {
            logger->error("Monthly fee cannot be negative: {}", fee);
            throw invalid_argument("Monthly fee cannot be negative");
        }
        monthlyFee = fee;
    }

    virtual string toString() const = 0;

    virtual string serialize() const {
        stringstream ss;
        ss << name << "," << fixed << setprecision(6) << monthlyFee << "," << clientCount;
        return ss.str();
    }

    static Tariff* deserialize(const string& data);
};

class PrepaidTariff : public Tariff {
private:
    double callRate;

public:
    PrepaidTariff(const string& name, double monthlyFee, double callRate)
        : Tariff(name, monthlyFee), callRate(callRate) {}

    double getCallRate() const { return callRate; }

    void setCallRate(double rate) {
        if (rate < 0) {
            logger->error("Call rate cannot be negative: {}", rate);
            throw invalid_argument("Call rate cannot be negative");
        }
        callRate = rate;
    }

    string toString() const override {
        return "Prepaid Tariff: " + getName() +
            ", Monthly Fee: " + to_string(getMonthlyFee()) +
            ", Call Rate: " + to_string(callRate) + "\n";
    }

    string serialize() const override {
        stringstream ss;
        ss << "Prepaid," << Tariff::serialize() << "," << fixed << setprecision(6) << callRate;
        return ss.str();
    }

    static Tariff* deserialize(const string& data) {
        stringstream ss(data);
        string name;
        double monthlyFee, callRate;
        getline(ss, name, ',');
        ss >> monthlyFee;
        ss.ignore(1, ',');
        ss >> callRate;
        return new PrepaidTariff(name, monthlyFee, callRate);
    }
};

class PostpaidTariff : public Tariff {
private:
    double includedMinutes;

public:
    PostpaidTariff(const string& name, double monthlyFee, double includedMinutes)
        : Tariff(name, monthlyFee), includedMinutes(includedMinutes) {}

    double getIncludedMinutes() const { return includedMinutes; }

    void setIncludedMinutes(double minutes) {
        if (minutes < 0) {
            logger->error("Included minutes cannot be negative: {}", minutes);
            throw invalid_argument("Included minutes cannot be negative");
        }
        includedMinutes = minutes;
    }

    string toString() const override {
        return "Postpaid Tariff: " + getName() +
            ", Monthly Fee: " + to_string(getMonthlyFee()) +
            ", Included Minutes: " + to_string(includedMinutes) + "\n";
    }

    string serialize() const override {
        stringstream ss;
        ss << "Postpaid," << Tariff::serialize() << "," << fixed << setprecision(6) << includedMinutes;
        return ss.str();
    }

    static Tariff* deserialize(const string& data) {
        stringstream ss(data);
        string name;
        double monthlyFee, includedMinutes;
        getline(ss, name, ',');
        ss >> monthlyFee;
        ss.ignore(1, ',');
        ss >> includedMinutes;
        return new PostpaidTariff(name, monthlyFee, includedMinutes);
    }
};

Tariff* Tariff::deserialize(const string& data) {
    stringstream ss(data);
    string type;
    getline(ss, type, ',');

    if (type == "Prepaid") {
        return PrepaidTariff::deserialize(data);
    }
    else if (type == "Postpaid") {
        return PostpaidTariff::deserialize(data);
    }
    return nullptr;
}

class TariffService {
private:
    vector<Tariff*> tariffs;
    const string dataPath = "./data/";

    void createDirectoryIfNotExists() {
#ifdef _WIN32
        if (_mkdir(dataPath.c_str()) != 0 && errno != EEXIST) {
            logger->error("Failed to create directory: {}", dataPath);
        }
#else
        if (mkdir(dataPath.c_str(), 0777) != 0 && errno != EEXIST) {
            logger->error("Failed to create directory: {}", dataPath);
        }
#endif
    }

public:
    TariffService() {
        createDirectoryIfNotExists();
    }

    ~TariffService() {
        for (auto tariff : tariffs) {
            delete tariff;
        }
    }

    void addTariff(Tariff* tariff) {
        tariffs.push_back(tariff);
        logger->info("Added tariff: {}", tariff->getName());
    }

    void loadTariffs(const string& filename) {
        ifstream file(dataPath + filename);
        if (!file.is_open()) {
            logger->error("Error opening file: {}", filename);
            throw runtime_error("Unable to open file: " + filename);
        }

        string line;
        if (!getline(file, line)) {
            logger->error("File is empty: {}", filename);
            throw runtime_error("File is empty: " + filename);
        }

        do {
            Tariff* tariff = Tariff::deserialize(line);
            if (tariff) {
                addTariff(tariff);
            }
        } while (getline(file, line));
    }

    void saveTariffs(const string& filename) const {
        if (tariffs.empty()) {
            logger->warn("No tariffs to save.");
            throw runtime_error("No tariffs to save.");
        }

        ofstream file(dataPath + filename);
        if (!file.is_open()) {
            logger->error("Error opening file for writing: {}", filename);
            throw runtime_error("Unable to open file for writing: " + filename);
        }

        for (const auto& tariff : tariffs) {
            file << tariff->serialize() << endl;
            logger->info("Serialized tariff: {}", tariff->getName());
        }
    }

    int calculateTotalClients() const {
        int total = 0;
        for (const auto& tariff : tariffs) {
            total += tariff->getClientCount();
        }
        logger->info("Total clients calculated: {}", total);
        return total;
    }

    void sortTariffsByMonthlyFee() {
        sort(tariffs.begin(), tariffs.end(), [](Tariff* a, Tariff* b) {
            return a->getMonthlyFee() < b->getMonthlyFee();
            });
        logger->info("Sorted tariffs by monthly fee");
    }

    vector<Tariff*> findTariffsWithinRange(double min, double max) const {
        vector<Tariff*> result;
        for (const auto& tariff : tariffs) {
            if (tariff->getMonthlyFee() >= min && tariff->getMonthlyFee() <= max) {
                result.push_back(tariff);
            }
        }
        logger->info("Found {} tariffs within range: [{}, {}]", result.size(), min, max);
        return result;
    }

    void printTariffs() const {
        for (const auto& tariff : tariffs) {
            logger->info("Tariff: {}", tariff->toString());
        }
    }
};

TEST(TariffTests, PrepaidSerialization) {
    PrepaidTariff tariff("Prepaid Plan", 10.0, 0.5);
    string expected = "Prepaid,Prepaid Plan,10.000000,0,0.500000";
    EXPECT_EQ(tariff.serialize(), expected);
}

TEST(TariffTests, PostpaidSerialization) {
    PostpaidTariff tariff("Postpaid Plan", 20.0, 100);
    string expected = "Postpaid,Postpaid Plan,20.000000,0,100.000000";
    EXPECT_EQ(tariff.serialize(), expected);
}

TEST(TariffServiceTests, LoadTariffs_FileNotFound) {
    TariffService service;
    EXPECT_THROW(service.loadTariffs("nonexistent_file.txt"), runtime_error);
}

TEST(TariffServiceTests, LoadTariffs_EmptyFile) {
    TariffService service;
    ofstream file("./data/empty_file.txt");
    file.close();

    EXPECT_THROW(service.loadTariffs("empty_file.txt"), runtime_error);
}

TEST(TariffServiceTests, SaveTariffs_NoTariffs) {
    TariffService service;
    EXPECT_THROW(service.saveTariffs("test_save_tariffs.txt"), runtime_error);
}

TEST(TariffServiceTests, CalculateTotalClients) {
    TariffService service;
    PrepaidTariff* tariff = new PrepaidTariff("Prepaid Plan A", 10.0, 0.5);
    tariff->incrementClientCount();
    service.addTariff(tariff);

    EXPECT_EQ(service.calculateTotalClients(), 1);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    spdlog::set_level(spdlog::level::info);
    return RUN_ALL_TESTS();
}