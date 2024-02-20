#ifndef BANKINGAPP_H
#define BANKINGAPP_H

#include <mariadb/conncpp.hpp> // Use MariaDB Connector/C++ header

class BankingApp {
private:
    std::unique_ptr<sql::Connection> con; // Keep using smart pointers for resource management

public:
    BankingApp();
    void connectToDatabase();
    sql::Connection* getConnection();
};

#endif // BANKINGAPP_H

