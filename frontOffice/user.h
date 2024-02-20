#ifndef USER_H
#define USER_H

#include <mariadb/conncpp.hpp> // Include MariaDB Connector/C++ header
#include <string>
#include <vector>

class User {
private:
    sql::Connection* con; // Using sql namespace from MariaDB Connector/C++
    int userID;
    bool isAuthenticated;

public:
    struct Account {
        std::string name;
        double balance;
        int id;
    };
    explicit User(sql::Connection* connection);

    bool authenticate(const std::string& login, const std::string& password);
    double getAccountBalance();
    void updateAccountBalance(double amount, int accountId);
    std::vector<Account> listAccounts();
};

#endif // USER_H

