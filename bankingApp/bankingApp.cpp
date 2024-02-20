#include "bankingApp.h"
#include <mariadb/conncpp/Driver.hpp>
#include <iostream>

BankingApp::BankingApp() {
    connectToDatabase();
}

void BankingApp::connectToDatabase() {
    try {
        // Use the MariaDB Connector/C++ to create a connection
        sql::Driver* driver = sql::mariadb::get_driver_instance();
        // Create a properties object for connection parameters
        sql::SQLString url("tcp://127.0.0.1:3306");
        sql::Properties properties({
            {"user", ""},
            {"password", ""}
        });
        // Connect to the database using the properties
        con.reset(driver->connect(url, properties));
        con->setSchema("banque"); // Name of your database
    } catch (sql::SQLException &e) {
        std::cerr << "Database connection failed: " << e.what() << std::endl;
        exit(1);
    }
}

sql::Connection* BankingApp::getConnection() {
    return con.get();
}

