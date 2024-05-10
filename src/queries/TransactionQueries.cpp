#include "../../include/queries/TransactionQueries.h"
#include "../../include/queries/AccountQueries.h"
#include "../../include/queries/UserQueries.h"
#include <iostream>

TransactionQueries::TransactionQueries(Connection* connection) : conn(connection) {}

TransactionQueries::~TransactionQueries() = default;

bool TransactionQueries::recordTransaction(int userId, const string& source, const string& destination, double amount,
                                           double oldBalanceSrc, double newBalanceSrc,
                                           double oldBalanceDest, double newBalanceDest) {
    auto conn = DatabaseManager::getInstance()->getConnection();
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement(
                "INSERT INTO historic (date, user_id, source, destination, amount, old_balance_src, new_balance_src, old_balance_dest, new_balance_dest) "
                "VALUES (CURDATE(), ?, ?, ?, ?, ?, ?, ?, ?);"));
        pstmt->setInt(1, userId);
        pstmt->setString(2, source);
        pstmt->setString(3, destination);
        pstmt->setDouble(4, amount);
        pstmt->setDouble(5, oldBalanceSrc);
        pstmt->setDouble(6, newBalanceSrc);
        pstmt->setDouble(7, oldBalanceDest);
        pstmt->setDouble(8, newBalanceDest);
        pstmt->executeUpdate();
        return true;
    } catch (SQLException& e) {
        cerr << "Failed to record transaction: " << e.what() << endl;
        return false;
    }
}


vector<TransactionModel> TransactionQueries::getTransactionHistory(int userId) {
    vector<TransactionModel> history;
    UserQueries userQueries(conn); // Assuming UserQueries can access the same connection

    try {
        auto pstmt = conn->prepareStatement(
                "SELECT h.date, h.source, h.destination, h.amount, "
                "h.old_balance_src, h.new_balance_src, h.old_balance_dest, h.new_balance_dest, u.id "
                "FROM historic h JOIN users u ON h.user_id = u.id "
                "WHERE h.user_id = ? ORDER BY h.date DESC;"); // WHERE clause to filter by user ID

        pstmt->setInt(1, userId); // Set the user ID for the query
        auto res = pstmt->executeQuery();

        while (res->next()) {
            std::string fullName = userQueries.getConjointName(res->getInt("id"));
            history.emplace_back(
                    res->getString("date").c_str(),
                    res->getString("source").c_str(),
                    res->getString("destination").c_str(),
                    res->getDouble("amount"),
                    res->getDouble("old_balance_src"),
                    res->getDouble("new_balance_src"),
                    res->getDouble("old_balance_dest"),
                    res->getDouble("new_balance_dest"),
                    fullName
            );
        }
    } catch (const SQLException &e) {
        cerr << "Failed to get transaction history for user ID " << userId << ": " << e.what() << endl;
    }
    return history;
}

vector<TransactionModel> TransactionQueries::getAllTransactionHistory() {
    vector<TransactionModel> history;
    UserQueries userQueries(conn);
    try {
        auto pstmt = conn->prepareStatement(
                "SELECT h.date, h.source, h.destination, h.amount, "
                "h.old_balance_src, h.new_balance_src, h.old_balance_dest, h.new_balance_dest, u.id "
                "FROM historic h JOIN users u ON h.user_id = u.id ORDER BY h.date DESC;");

        auto res = pstmt->executeQuery();
        while (res->next()) {
            std::string fullName = userQueries.getConjointName(res->getInt("id"));  // Assuming getUserFullName is a method in UserQueries
            history.emplace_back(
                    res->getString("date").c_str(),
                    res->getString("source").c_str(),
                    res->getString("destination").c_str(),
                    res->getDouble("amount"),
                    res->getDouble("old_balance_src"),
                    res->getDouble("new_balance_src"),
                    res->getDouble("old_balance_dest"),
                    res->getDouble("new_balance_dest"),
                    fullName
            );
        }
    } catch (const SQLException& e) {
        cerr << "Failed to get all transaction history: " << e.what() << endl;
    }
    return history;
}

vector<TransactionModel> TransactionQueries::getTransactionsByAccountId(string accountNumber) {
    vector<TransactionModel> transactions;
    UserQueries userQueries(conn);
    try {
        // Ensure your SQL query selects the user_id if you need it for fetching additional details.
        auto pstmt = conn->prepareStatement(
                "SELECT h.date, h.source, h.destination, h.amount, h.old_balance_src, h.new_balance_src, h.old_balance_dest, h.new_balance_dest, h.user_id "
                "FROM historic h WHERE h.source = ? OR h.destination = ? ORDER BY h.date DESC;"
        );
        pstmt->setString(1, accountNumber);
        pstmt->setString(2, accountNumber);
        auto res = pstmt->executeQuery();
        while (res->next()) {
            int userId = res->getInt("user_id");  // Retrieve the user ID from the query
            std::string fullName = userQueries.getConjointName(userId);  // Fetch the full name using the user ID

            transactions.emplace_back(
                    res->getString("date").c_str(),
                    res->getString("source").c_str(),
                    res->getString("destination").c_str(),
                    res->getDouble("amount"),
                    res->getDouble("old_balance_src"),
                    res->getDouble("new_balance_src"),
                    res->getDouble("old_balance_dest"),
                    res->getDouble("new_balance_dest"),
                    fullName  // Assuming you have a structure to store this info in TransactionModel
            );
        }
    } catch (const SQLException& e) {
        cerr << "Failed to get transactions for account number: " << accountNumber << ". Error: " << e.what() << endl;
    }
    return transactions;
}


bool TransactionQueries::removeUserHistoricEntries(int userId) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("DELETE FROM historic WHERE user_id = ?;"));
        pstmt->setInt(1, userId);
        pstmt->executeUpdate();
        return true;
    } catch (const SQLException& e) {
        cerr << "Failed to remove historic entries for user: " << e.what() << endl;
        return false;
    }
}