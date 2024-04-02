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
            {"user", "root"},
            {"password", "ROOT"}
        });

          // Connect to the database using the properties
        con.reset(driver->connect(url, properties));
        con->setSchema("banque"); // Name of your database

        sql::Statement *stmt = con->createStatement();

        // Create the "banque" database, if not exists
        stmt->execute("CREATE DATABASE IF NOT EXISTS banque");

        // Select the "banque" database
        stmt->execute("USE banque");

        // Create the tables if they don't exist
        stmt->execute("CREATE TABLE IF NOT EXISTS Utilisateur (ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY, Nom VARCHAR(50) NOT NULL, Prenom VARCHAR(50) NOT NULL, Login VARCHAR(50) NOT NULL, Password VARCHAR(50) NOT NULL)");

        stmt->execute("CREATE TABLE IF NOT EXISTS CompteCourant (ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY, ID_Utilisateur INT NOT NULL, Solde DOUBLE NOT NULL, Name VARCHAR(50) NOT NULL)");

        stmt->execute("CREATE TABLE IF NOT EXISTS banquiers (ID INT NOT NULL AUTO_INCREMENT PRIMARY KEY, Login VARCHAR(50) NOT NULL, Password VARCHAR(50) NOT NULL)");


      
    } catch (sql::SQLException &e) {
        std::cerr << "Database connection failed: " << e.what() << std::endl;
        exit(1);
    }
}

sql::Connection* BankingApp::getConnection() {
    return con.get();
}

