#include "backOffice.h"
#include <iostream>

BackOffice::BackOffice(sql::Connection* connection) : con(connection) {}

bool BackOffice::authenticate(const std::string& login, const std::string& password) {
    try {
        auto pstmt = con->prepareStatement("SELECT ID FROM banquiers WHERE Login = ? AND Password = ?");
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

void BackOffice::createUser(const std::string& nom, const std::string& prenom, const std::string& login, const std::string& password) {
    try {
        // Prepare the SQL statement to insert a new user
        auto pstmt = con->prepareStatement("INSERT INTO Utilisateur (Nom, Prenom, Login, Password) VALUES (?, ?, ?, ?)");

        // Bind the parameters to the SQL query to avoid SQL injection
        pstmt->setString(1, nom);
        pstmt->setString(2, prenom);
        pstmt->setString(3, login);
        pstmt->setString(4, password);

        // Execute the SQL statement
        pstmt->executeUpdate();

        std::cout << "User successfully created." << std::endl;
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        // Handle duplicate login error or any other SQL exceptions appropriately
        if (e.getErrorCode() == 1062) { // Duplicate entry for key 'Login'
            std::cerr << "Error: The provided login is already in use. Please choose a different login." << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


void BackOffice::createAndAssociateAccount(const std::string& userName, const std::string& accountName, double initialBalance) {
    try {
        // Step 1: Check if user exists
        auto pstmtUser = con->prepareStatement("SELECT ID FROM Utilisateur WHERE Login = ?");
        pstmtUser->setString(1, userName);
        auto resUser = pstmtUser->executeQuery();

        if (resUser->next()) {
            int userID = resUser->getInt("ID");

            // Since users can have multiple CompteCourant, proceed to create a new one without checking for existing accounts

            // Step 2: Create and associate CompteCourant
            auto pstmtCreateAccount = con->prepareStatement("INSERT INTO CompteCourant (ID_Utilisateur, Solde, Name) VALUES (?, ?, ?)");
            pstmtCreateAccount->setInt(1, userID);
            pstmtCreateAccount->setDouble(2, initialBalance);
            // Here, you might want to allow specifying a unique name for each account or generate it based on some logic
            pstmtCreateAccount->setString(3, accountName); // Adjust as necessary to ensure uniqueness or meaningful naming
            pstmtCreateAccount->executeUpdate();

            std::cout << "New CompteCourant successfully created and associated with user " << userName << std::endl;
        } else {
            std::cerr << "User does not exist." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }
}

std::vector<BackOffice::User> BackOffice::listUsers() {
    std::vector<User> users;

    try {
        // Prepare the SQL statement to select login and ID from Utilisateur
        auto pstmt = con->prepareStatement("SELECT ID, Login FROM Utilisateur");
        auto res = pstmt->executeQuery();

        // Check if the result set is empty
        if (!res->next()) {
            return users; // Return the empty vector
        }

        // Reset the cursor of the result set
        res->beforeFirst();

        while (res->next()) {
            // Retrieve each user's ID and login and add to the list
            User user;
            user.id = res->getInt("ID");
            user.login = res->getString("Login");
            users.push_back(user);

            // Or if using a pair:
            // users.emplace_back(res->getInt("ID"), res->getString("Login"));
        }
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
    }

    return users;
}

int BackOffice::checkAccountTypes(const std::string& loginUser) {
    int accountTypes = 0;
    int userID = -1;

    try {
        // Get the userID from loginUser
        auto pstmtUser = con->prepareStatement("SELECT ID FROM Utilisateur WHERE Login = ?");
        pstmtUser->setString(1, loginUser);
        auto resUser = pstmtUser->executeQuery();

        if (resUser->next()) {
            userID = resUser->getInt("ID");
        } else {
            std::cerr << "User login not found." << std::endl;
            return accountTypes; // Return 0, meaning no accounts found.
        }

        // Now that we have userID, we can check for "Livret C"
        auto pstmtLivretC = con->prepareStatement("SELECT ID FROM CompteCourant WHERE ID_Utilisateur = ? AND Name = 'Livret C'");
        pstmtLivretC->setInt(1, userID);
        auto resLivretC = pstmtLivretC->executeQuery();
        if (resLivretC->next()) {
            accountTypes += 2; // Add 2 to the result if "Livret C" exists
        }

        // Check for "PEL"
        auto pstmtPEL = con->prepareStatement("SELECT ID FROM CompteCourant WHERE ID_Utilisateur = ? AND Name = 'PEL'");
        pstmtPEL->setInt(1, userID);
        auto resPEL = pstmtPEL->executeQuery();
        if (resPEL->next()) {
            accountTypes += 1; // Add 1 to the result if "PEL" exists
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Error checking account types: " << e.what() << std::endl;
    }

    return accountTypes;
}




