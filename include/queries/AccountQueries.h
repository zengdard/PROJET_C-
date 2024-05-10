#ifndef ACCOUNTQUERIES_H
#define ACCOUNTQUERIES_H

#include <string>
#include <vector>
#include <mariadb/conncpp.hpp>
#include "../../include/models/AccountModel.h"


using namespace sql;
using namespace std;

class AccountQueries {
public:
    explicit AccountQueries(Connection* connection);
    bool createAccount(int userId, const string& accountType, const string& accountNumber, double balance, double dailyLimit);
    bool removeAccount(const string& accountNumber);
    bool removeAllAccountsForUser(int userId);
    bool doesAccountNumberExist(const string& accountNumber);
    vector<int> getUserIdAccounts(int userId);
    double getBalance(const string& accountNumber);
    double getDailyLimit(const string& accountNumber);
    string getAccountNumber(int userId, const string& accountType);
    string getAccountType(const string& accountNumber);
    string getLastTransaction(const string& accountNumber);
    vector<string> getAllAccounts(int userId);

    bool accountExists(int userId, const string &accountType);

    bool updateAccountBalance(int userId, const string &accountType, double newBalance);

    string getLastTransactionDate(const std::string& accountNumber);
    bool removeAccountByType(int userId, const string &accountType);
    bool updateAccountBalance(const string& accountNumber, double newBalance);
    vector<AccountModel> getAccountsByUserId(int userId);
    string generateUniqueAccountNumber();

private:
    Connection* conn;
};

#endif // ACCOUNTQUERIES_H
