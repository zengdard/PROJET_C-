#ifndef ACCOUNTMODEL_H
#define ACCOUNTMODEL_H

#include <string>

using namespace std;

class AccountModel {
public:
    int id;                        // Account ID
    int userId;                    // Associated User ID
    string accountType;       // Type of the account
    string accountNumber;     // Account number (unique)
    double balance;                // Current balance
    double dailyLimit;             // Daily transaction limit
    string lastTransactionDate; // Date of the last transaction

    // Default constructor
    AccountModel() = default;

    // Constructor for initializing an AccountModel object
    AccountModel(int id, int userId, const string& accountType, const string& accountNumber,
                 double balance, double dailyLimit, const string& lastTransactionDate)
            : id(id), userId(userId), accountType(accountType), accountNumber(accountNumber),
              balance(balance), dailyLimit(dailyLimit), lastTransactionDate(lastTransactionDate) {}

    AccountModel(const AccountModel& other) = default; // Copy constructor
    ~AccountModel() = default; // Destructor
};

#endif //ACCOUNTMODEL_H
