#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

using namespace std;

auto logger = spdlog::stdout_color_mt("logger");


const string server = "tcp://localhost:3306";
const string username = "root";
const string password = "12101906Nn#06";

int main()
{
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;

    try
    {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
        logger->info("Connected");
    }
    catch (sql::SQLException e)
    {
        logger->warn("Could not connect to server. Error message: {}", e.what());
        system("pause");
        exit(1);
    }

    con->setSchema("lab8");

    
    return 0;
}