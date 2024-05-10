#ifndef USERQUERIES_H
#define USERQUERIES_H

#include <string>
#include <vector>
#include "../../../include/db/DatabaseManager.h"
#include "../models/UserModel.h"
#include <mariadb/conncpp/Connection.hpp>
#include <mariadb/conncpp/ResultSet.hpp>

using namespace std;
using namespace sql;

class UserQueries {
public:
    explicit UserQueries(Connection* connection);
    bool createUser(const int &conjoint, const string &login, const string &password, const string &firstname,
                    const string &lastname, const string &address, const string &postalcode, const string &city,
                    const string &role);
    bool removeUser(int userId);
    int getUserId(const string& login);
    unique_ptr<UserModel> getUser(const int& userId);
    bool updateUser(int userId, const map<string, string>& updates);
    vector<UserModel> getAllUsers();
    string getRole(const string& login);
    bool checkLogin(const string& login, const string& password);  // Make public

    int getLastInsertedUserId();

    string getConjointName(int userId);

private:
    Connection* conn;
    static string hashPassword(const string& password);

    bool prepareForDeletion(int userId);
};

#endif // USERQUERIES_H
