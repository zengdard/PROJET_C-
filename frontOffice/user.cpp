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

void User::updateAccountBalance(double amount, int accountId, std::string accountType) {
    if (!isAuthenticated) {
        std::cerr << "User not authenticated." << std::endl;
        return;
    }

    // Vérifier les règles spécifiques au type de compte
    if (accountType == "Livret C") {
        // Vérifier que le montant est supérieur ou égal à 1 euro
        if (amount < 1) {
            std::cout << "Le montant minimum pour un virement sur un Livret C est de 1 euro." << std::endl;
            return;
        }

        // Vérifier le solde actuel et le montant maximum autorisé pour un virement
        try {
            auto pstmt = con->prepareStatement("SELECT Solde FROM LivretC WHERE ID_Utilisateur = ? AND ID = ?");
            pstmt->setInt(1, userID);
            pstmt->setInt(2, accountId);
            auto res = pstmt->executeQuery();
            if (res->next()) {
                double currentBalance = res->getDouble("Solde");
                if (currentBalance + amount > 10000) {
                    std::cout << "Le solde maximum d'un Livret C est de 10 000 euros." << std::endl;
                    return;
                }
                if (amount > currentBalance * 0.05) {
                    std::cout << "Le montant maximum pour un virement sur un Livret C est de 5% du solde actuel." << std::endl;
                    return;
                }
            } else {
                std::cerr << "Compte non trouvé." << std::endl;
                return;
            }
        } catch (sql::SQLException &e) {
            std::cerr << "Error checking account balance: " << e.what() << std::endl;
            return;
        }
    } else if (accountType == "PEL") {
        if (amount < 0) {
            std::cout << "Impossible de débiter le PEL. INTERDIT" << std::endl;
            return;
        }
        if (amount < 50) {
            std::cout << "Le montant minimum pour un dépôt sur un PEL est de 50 euros." << std::endl;
            return;
        }
    }

    // Mettre à jour le solde du compte
    try {
        if (accountType == "Livret C") {
            auto pstmt = con->prepareStatement("UPDATE LivretC SET Solde = Solde + ? WHERE ID_Utilisateur = ? AND ID = ?");
            pstmt->setDouble(1, amount);
            pstmt->setInt(2, userID);
            pstmt->setInt(3, accountId);
            pstmt->executeUpdate();
        } else if (accountType == "PEL") {
            auto pstmt = con->prepareStatement("UPDATE PEL SET Solde = Solde + ? WHERE ID_Utilisateur = ? AND ID = ?");
            pstmt->setDouble(1, amount);
            pstmt->setInt(2, userID);
            pstmt->setInt(3, accountId);
            pstmt->executeUpdate();
        } else {
            auto pstmt = con->prepareStatement("UPDATE Compte SET Solde = Solde + ? WHERE ID_Utilisateur = ? AND ID = ?");
            pstmt->setDouble(1, amount);
            pstmt->setInt(2, userID);
            pstmt->setInt(3, accountId);
            pstmt->executeUpdate();
        }
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