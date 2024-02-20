#include "user.h"
#include <mariadb/conncpp/PreparedStatement.hpp> // Adjust include path for prepared statements
#include <mariadb/conncpp/ResultSet.hpp> // Adjust include path for result sets
#include <iostream>

User::User(sql::Connection* connection) : con(connection), userID(-1), isAuthenticated(false) {}

bool User::authenticate(const std::string& login, const std::string& password) {
    try {
        auto pstmt = con->prepareStatement("SELECT ID FROM Utilisateur WHERE Login = ? AND Password = ?");
        pstmt->setString(1, login);
        pstmt->setString(2, password);
        auto res = pstmt->executeQuery();

        if (res->next()) {
            userID = res->getInt("ID");
            isAuthenticated = true;
            return true;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Authentication failed: " << e.what() << std::endl;
    }
    return false;
}

double User::getAccountBalance() {
    if (!isAuthenticated) {
        std::cerr << "User not authenticated." << std::endl;
        return 0.0;
    }

    try {
        auto pstmt = con->prepareStatement("SELECT Solde FROM CompteCourant WHERE ID_Utilisateur = ?");
        pstmt->setInt(1, userID);
        auto res = pstmt->executeQuery();

        if (res->next()) {
            return res->getDouble("Solde");
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error getting account balance: " << e.what() << std::endl;
    }
    return 0.0;
}

void User::updateAccountBalance(double amount, int accountId) {
    if (!isAuthenticated) {
        std::cerr << "User not authenticated." << std::endl;
        return;
    }

    try {
        auto pstmt = con->prepareStatement("UPDATE CompteCourant SET Solde = Solde + ? WHERE ID_Utilisateur = ? AND ID = ?");
        pstmt->setDouble(1, amount);
        pstmt->setInt(2, userID);
        pstmt->setInt(3, accountId);
        pstmt->executeUpdate();
    } catch (sql::SQLException &e) {
        std::cerr << "Error updating account balance: " << e.what() << std::endl;
    }
}

std::vector<User::Account> User::listAccounts() {
    std::vector<Account> accounts;

    if (!isAuthenticated) {
        std::cerr << "User not authenticated." << std::endl;
        return accounts; // Return an empty vector
    }

    try {
        auto pstmt = con->prepareStatement("SELECT Name, Solde, ID FROM CompteCourant WHERE ID_Utilisateur = ?");
        pstmt->setInt(1, userID);
        auto res = pstmt->executeQuery();

        while (res->next()) {
            Account account;
            account.name = res->getString("Name");
            account.id = res->getInt("ID");
            account.balance = res->getDouble("Solde");
            accounts.push_back(account);
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error listing accounts: " << e.what() << std::endl;
    }

    return accounts;
}