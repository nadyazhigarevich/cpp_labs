#include "pch.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <vector>
#include <string>

auto logger = spdlog::stdout_color_mt("test_logger");


class Park {
public:
    class Attraction {
    private:
        std::string name;
        std::string hours;
        double price;

    public:
        Attraction(const std::string& name, const std::string& hours, double price)
            : name(name), hours(hours), price(price) {}

        std::string toString() const {
            return "Attraction Name: " + name + "\n" +
                "Operating Hours: " + hours + "\n" +
                "Price: $" + std::to_string(price) + "\n";
        }
    };

    void addAttraction(const std::string& name, const std::string& hours, double price) {
        logger->info("Trying to add attraction.");

        attractions.emplace_back(name, hours, price);
    }

    std::string getAttractionsInfo() const {
        logger->info("Trying get info about all attractions.");

        std::string info = "Attractions in the Park:\n";
        for (const auto& attraction : attractions) {
            info += attraction.toString();
            info += "--------------------------\n";
        }
        return info;
    }

private:
    std::vector<Attraction> attractions; 
    std::string parkName;                
    std::string location;                
    int totalAttractions;                 

public:

    Park(const std::string& name, const std::string& loc)
        : parkName(name), location(loc), totalAttractions(0) {}

    std::string getParkName() const {
        return parkName;
    }

    std::string getLocation() const {
        return location;
    }

    void updateTotalAttractions() {
        totalAttractions = attractions.size();
    }
};

class ParkTest : public ::testing::Test {
protected:
    Park park;

    ParkTest() : park("Fun Land", "123 Amusement Ave") {}
};

TEST_F(ParkTest, GetParkName) {
    std::string expected = "Fun Land";
    std::string actual = park.getParkName();

    EXPECT_EQ(actual, expected);
}

TEST_F(ParkTest, GetLocation) {
    std::string expected = "123 Amusement Ave";
    std::string actual = park.getLocation();

    EXPECT_EQ(actual, expected);
}

TEST_F(ParkTest, UpdateTotalAttractions) {
    park.addAttraction("Haunted House", "12 PM - 10 PM", 4.0);
    park.updateTotalAttractions();

    std::string actualInfo = park.getAttractionsInfo();
    std::string expectedStart = "Attractions in the Park:";

    EXPECT_EQ(actualInfo.find(expectedStart), 0);
}

TEST_F(ParkTest, CheckAttractionFormat) {
    park.addAttraction("Merry-Go-Round", "10 AM - 7 PM", 4.0);
    std::string actualInfo = park.getAttractionsInfo();

    std::string expectedName = "Attraction Name: Merry-Go-Round";
    std::string expectedHours = "Operating Hours: 10 AM - 7 PM";
    std::string expectedPrice = "Price: $4.000000";

    EXPECT_TRUE(actualInfo.find(expectedName) != std::string::npos);
    EXPECT_TRUE(actualInfo.find(expectedHours) != std::string::npos);
    EXPECT_TRUE(actualInfo.find(expectedPrice) != std::string::npos);
}

TEST_F(ParkTest, HandlingEmptyAttractions) {
    std::string expected = "Attractions in the Park:\n";
    std::string actual = park.getAttractionsInfo();

    EXPECT_EQ(actual, expected);
}