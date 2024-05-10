#include <iostream>
#include "../../include/queries/AccountQueries.h"


AccountQueries::AccountQueries(Connection* connection) : conn(connection) {}

bool AccountQueries::createAccount(int userId, const string& accountType, const string& accountNumber, double balance, double dailyLimit) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("INSERT INTO accounts (user_id, account_type, account_number, balance, daily_limit) VALUES (?, ?, ?, ?, ?);"));
        pstmt->setInt(1, userId);
        pstmt->setString(2, accountType);
        pstmt->setString(3, accountNumber);
        pstmt->setDouble(4, balance);
        pstmt->setDouble(5, dailyLimit);
        pstmt->executeUpdate();
        return true;
    } catch (SQLException& e) {
        cerr << "Failed to create account: " << e.what() << endl;
        return false;
    }
}

bool AccountQueries::removeAccount(const string& accountNumber) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("DELETE FROM accounts WHERE account_number = ?;"));
        pstmt->setString(1, accountNumber);
        int affected = pstmt->executeUpdate();
        return affected > 0;
    } catch (SQLException& e) {
        cerr << "Failed to remove account: " << e.what() << endl;
        return false;
    }
}

bool AccountQueries::accountExists(int userId, const string& accountType) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT COUNT(*) FROM accounts WHERE user_id = ? AND account_type = ?;"));
        pstmt->setInt(1, userId);
        pstmt->setString(2, accountType);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next() && res->getInt(1) > 0) {
            return true;
        }
        return false;
    } catch (SQLException& e) {
        cerr << "Failed to check if account exists: " << e.what() << endl;
        return false;
    }
}

bool AccountQueries::updateAccountBalance(int userId, const string& accountType, double newBalance) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("UPDATE accounts SET balance = ? WHERE user_id = ? AND account_type = ?;"));
        pstmt->setDouble(1, newBalance);
        pstmt->setInt(2, userId);
        pstmt->setString(3, accountType);
        pstmt->executeUpdate();
        return true;
    } catch (SQLException& e) {
        cerr << "Failed to update account balance: " << e.what() << endl;
        return false;
    }
}

bool AccountQueries::removeAccountByType(int userId, const string& accountType) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("DELETE FROM accounts WHERE user_id = ? AND account_type = ?;"));
        pstmt->setInt(1, userId);
        pstmt->setString(2, accountType);
        int affectedRows = pstmt->executeUpdate();
        return affectedRows > 0;
    } catch (SQLException& e) {
        cerr << "Failed to remove account by type: " << e.what() << endl;
        return false;
    }
}

bool AccountQueries::updateAccountBalance(const string& accountNumber, double newBalance) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement(
                "UPDATE accounts SET balance = ?, date_last_transaction = CURDATE() WHERE account_number = ?;"
        ));
        pstmt->setDouble(1, newBalance);
        pstmt->setString(2, accountNumber);
        int result = pstmt->executeUpdate();
        if (result == 0) {
            cerr << "No rows updated, check if the account number exists and is correct: " << accountNumber << endl;
            return false;
        }
        return true;
    } catch (SQLException& e) {
        cerr << "Failed to update account balance: " << e.what() << " SQLState: " << e.getSQLState() << endl;
        return false;
    }
}


string AccountQueries::getLastTransactionDate(const string& accountNumber) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement(
                "SELECT MAX(date) AS last_transaction FROM historic WHERE source = ? OR destination = ?;"
        ));
        pstmt->setString(1, accountNumber);
        pstmt->setString(2, accountNumber);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            string lastDate = res->getString("last_transaction").c_str();
            if (res->wasNull()) {  // Correct method to check for null values in ResultSet
                return ""; // No transactions found
            }
            return lastDate;  // Use the string directly
        }
        return "";  // No transactions found
    } catch (SQLException& e) {
        cerr << "Failed to get the last transaction date: " << e.what() << endl;
        throw runtime_error("Database query failed in getLastTransactionDate");
    }
}

vector<AccountModel> AccountQueries::getAccountsByUserId(int userId) {
    vector<AccountModel> accounts;
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement(
                "SELECT id, user_id, account_type, account_number, balance, daily_limit, date_last_transaction "
                "FROM accounts WHERE user_id = ?;"
        ));
        pstmt->setInt(1, userId);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        while (res->next()) {
            accounts.emplace_back(
                    res->getInt("id"),
                    res->getInt("user_id"),
                    res->getString("account_type").c_str(),
                    res->getString("account_number").c_str(),
                    res->getDouble("balance"),
                    res->getDouble("daily_limit"),
                    res->getString("date_last_transaction").c_str()
            );
        }
    } catch (SQLException& e) {
        cerr << "Failed to get accounts by user ID: " << e.what() << endl;
    }
    return accounts;
}


string AccountQueries::generateUniqueAccountNumber() {
    string accountNumber;
    bool isUnique = false;
    do {
        long randomNumber = 1000000000 + rand() % 9000000000; // Ensure the number is always 10 digits
        accountNumber = to_string(randomNumber);
        isUnique = !doesAccountNumberExist(accountNumber);
    } while (!isUnique);
    return accountNumber;
}


bool AccountQueries::removeAllAccountsForUser(int userId) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("DELETE FROM accounts WHERE user_id = ?;"));
        pstmt->setInt(1, userId);
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (SQLException& e) {
        cerr << "Failed to remove accounts for user ID " << userId << ": " << e.what() << endl;
        return false;
    }
}

bool AccountQueries::doesAccountNumberExist(const string& accountNumber) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT COUNT(*) FROM accounts WHERE account_number = ?;"));
        pstmt->setString(1, accountNumber);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            return res->getInt(1) > 0; // Returns true if count > 0
        }
    } catch (SQLException& e) {
        cerr << "Failed to check account number existence: " << e.what() << endl;
    }
    return false;
}


vector<int> AccountQueries::getUserIdAccounts(int userId) {
    vector<int> accountIds;
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT id FROM accounts WHERE user_id = ?;"));
        pstmt->setInt(1, userId);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        while (res->next()) {
            accountIds.push_back(res->getInt("id"));
        }
    } catch (SQLException& e) {
        cerr << "Failed to get accounts for user ID " << userId << ": " << e.what() << endl;
    }
    return accountIds;
}


double AccountQueries::getBalance(const string& accountNumber) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT balance FROM accounts WHERE account_number = ?;"));
        pstmt->setString(1, accountNumber);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            return res->getDouble("balance");
        }
    } catch (SQLException& e) {
        cerr << "Failed to get balance: " << e.what() << endl;
    }
    return 0.0;
}

double AccountQueries::getDailyLimit(const string& accountNumber) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT daily_limit FROM accounts WHERE account_number = ?;"));
        pstmt->setString(1, accountNumber);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            return res->getDouble("daily_limit");
        }
    } catch (SQLException& e) {
        cerr << "Failed to get daily limit: " << e.what() << endl;
    }
    return 0.0;
}

string AccountQueries::getAccountNumber(int userId, const string& accountType) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT account_number FROM accounts WHERE user_id = ? AND account_type = ?;"));
        pstmt->setInt(1, userId);
        pstmt->setString(2, accountType);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            SQLString sqlStr = res->getString("account_number");
            return string(sqlStr.c_str());
        }
    } catch (SQLException& e) {
        cerr << "Failed to get account number: " << e.what() << endl;
    }
    return "";
}

string AccountQueries::getAccountType(const string& accountNumber) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT account_type FROM accounts WHERE account_number = ?;"));
        pstmt->setString(1, accountNumber);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            SQLString sqlStr = res->getString("account_type");
            return string(sqlStr.c_str());
        }
    } catch (SQLException& e) {
        cerr << "Failed to get account type: " << e.what() << endl;
    }
    return "";
}


string AccountQueries::getLastTransaction(const string& accountNumber) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT date_last_transaction FROM accounts WHERE account_number = ?;"));
        pstmt->setString(1, accountNumber);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            SQLString sqlStr = res->getString("date_last_transaction");
            return string(sqlStr.c_str());
        }
    } catch (SQLException& e) {
        cerr << "Failed to get the last transaction date: " << e.what() << endl;
    }
    return "";
}

vector<string> AccountQueries::getAllAccounts(int userId) {
    vector<string> accounts;
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT account_number FROM accounts WHERE user_id = ?;"));
        pstmt->setInt(1, userId);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        while (res->next()) {
            SQLString sqlStr = res->getString("account_number");
            string accountNumber = sqlStr.c_str();
            accounts.push_back(accountNumber);
        }
    } catch (SQLException& e) {
        cerr << "Failed to get accounts: " << e.what() << endl;
    }
    return accounts;
}