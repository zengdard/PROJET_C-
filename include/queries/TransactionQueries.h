#ifndef RENTALBANK_TRANSACTIONQUERIES_H
#define RENTALBANK_TRANSACTIONQUERIES_H

#include <mariadb/conncpp/Connection.hpp>
#include <mariadb/conncpp/ResultSet.hpp>
#include "../../../include/db/DatabaseManager.h"
#include "../models/TransactionModel.h"

using namespace std;
using namespace sql;

class TransactionQueries {
public:
    explicit TransactionQueries(Connection *connection);

    ~TransactionQueries();

    // Example method to record a transaction
    bool recordTransaction(int userId, const std::string& source, const std::string& destination, double amount,
                           double oldBalanceSrc, double newBalanceSrc,
                           double oldBalanceDest, double newBalanceDest);
    bool removeUserHistoricEntries(int userId);
    vector<TransactionModel> getTransactionHistory(int userId);
    vector<TransactionModel> getAllTransactionHistory();
    vector<TransactionModel> getTransactionsByAccountId(string accountNumber);


private:
    Connection* conn;  // Database connection

};



#endif //RENTALBANK_TRANSACTIONQUERIES_H