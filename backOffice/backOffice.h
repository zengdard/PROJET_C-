#ifndef BACKOFFICE_H
#define BACKOFFICE_H

#include <mariadb/conncpp.hpp>
#include <string>
#include <vector>

class BackOffice {
private:
    sql::Connection* con;
    int userID;
    bool isAuthenticated;


public:
    struct User {
        int id;
        std::string login;
    };
    explicit BackOffice(sql::Connection* connection);
    bool authenticate(const std::string& login, const std::string& password);
    void createUser(const std::string& nom, const std::string& prenom, const std::string& login, const std::string& password);
    void createAndAssociateAccount(const std::string& userName, const std::string& accountName, double initialBalance);
    std::vector<BackOffice::User> listUsers();
    int checkAccountTypes(const std::string& loginUser);
};

#endif // BACKOFFICE_H
