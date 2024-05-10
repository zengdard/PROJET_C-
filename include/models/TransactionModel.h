#ifndef TRANSACTIONMODEL_H
#define TRANSACTIONMODEL_H

#include <string>

using namespace std;

// Include necessary headers
#include <string>

class TransactionModel {
public:
    std::string date;
    std::string sourceAccount;
    std::string destinationAccount;
    double amount;
    double oldBalanceSrc;
    double newBalanceSrc;
    double oldBalanceDest;
    double newBalanceDest;
    string fullName;  // Full name of the person who made the transaction

    TransactionModel(const string& date, const string& sourceAccount, const string& destinationAccount,
                     double amount, double oldBalanceSrc, double newBalanceSrc,
                     double oldBalanceDest, double newBalanceDest, const string& fullName)
            : date(date), sourceAccount(sourceAccount), destinationAccount(destinationAccount),
              amount(amount), oldBalanceSrc(oldBalanceSrc), newBalanceSrc(newBalanceSrc),
              oldBalanceDest(oldBalanceDest), newBalanceDest(newBalanceDest), fullName(fullName) {}
};


#endif //TRANSACTIONMODEL_H
