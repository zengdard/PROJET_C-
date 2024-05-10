#include "../../include/db/DatabaseManager.h"

#include <mariadb/conncpp/Driver.hpp>
#include <iostream>

unique_ptr<DatabaseManager> DatabaseManager::instance = nullptr;  // Define the static member

DatabaseManager::DatabaseManager() {
    connectToDatabase();
}

DatabaseManager* DatabaseManager::getInstance() {
    if (!instance) {
        instance = make_unique<DatabaseManager>();
    }
    return instance.get();
}

void DatabaseManager::connectToDatabase() {
    try {
        Driver* driver = mariadb::get_driver_instance();
        SQLString url("tcp://127.0.0.1:3306");
        Properties properties({
                                      {"user", "root"},
                                      {"password", "toor"}
                              });
        con = unique_ptr<Connection>(driver->connect(url, properties));
        initializeDatabase();
    } catch (SQLException &e) {
        cerr << "Database connection failed: " << e.what() << endl;
        exit(1);
    }
}

void DatabaseManager::initializeDatabase() {
    try {
        unique_ptr<sql::Statement> stmt(con->createStatement());
        // Create the database if it does not exist
        stmt->execute("CREATE DATABASE IF NOT EXISTS ReetailBank");
        // Select the database
        con->setSchema("ReetailBank");
        // Create the users table
        stmt->execute("CREATE TABLE IF NOT EXISTS users ("
                      "id INT AUTO_INCREMENT PRIMARY KEY, "
                      "login VARCHAR(255) UNIQUE, "
                      "password VARCHAR(255), "
                      "firstname VARCHAR(255), "
                      "lastname VARCHAR(255), "
                      "address VARCHAR(255), "
                      "postalcode VARCHAR(20), "
                      "city VARCHAR(255), "
                      "role VARCHAR(50), "
                      "conjoint INT NULL, "
                      "FOREIGN KEY (conjoint) REFERENCES users(id)"
                      ");");


        // Create the accounts table
        stmt->execute("CREATE TABLE IF NOT EXISTS accounts ("
                      "id INT AUTO_INCREMENT PRIMARY KEY, "
                      "user_id INT, "
                      "account_type VARCHAR(50), "
                      "account_number VARCHAR(255) UNIQUE, "
                      "balance DECIMAL(15, 2), "
                      "daily_limit DECIMAL(15, 2), "
                      "date_last_transaction DATE, "
                      "FOREIGN KEY (user_id) REFERENCES users(id)"
                      ");");

        // Create the historic table
        stmt->execute("CREATE TABLE IF NOT EXISTS historic ("
                      "id INT AUTO_INCREMENT PRIMARY KEY, "
                      "date DATE, "
                      "user_id INT, "
                      "source VARCHAR(50), "
                      "destination VARCHAR(50), "
                      "amount DECIMAL(15, 2), "
                      "old_balance_src DECIMAL(15, 2), "
                      "new_balance_src DECIMAL(15, 2), "
                      "old_balance_dest DECIMAL(15, 2), "
                      "new_balance_dest DECIMAL(15, 2), "
                      "FOREIGN KEY (user_id) REFERENCES users(id)"
                      ");");

    } catch (sql::SQLException &e) {
        cerr << "Failed to initialize database: " << e.what() << endl;
        exit(1);
    }
}

Connection* DatabaseManager::getConnection() {
    return con.get();
}