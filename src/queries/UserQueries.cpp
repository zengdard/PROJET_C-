#include "../../include/queries/UserQueries.h"
#include "../../include/queries/AccountQueries.h"
#include "../../include/queries/TransactionQueries.h"

#include <iostream>
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>

UserQueries::UserQueries(Connection* connection) : conn(connection) {}

bool UserQueries::createUser(const int& conjoint, const string& login, const string& password, const string& firstname,
                             const string& lastname, const string& address, const string& postalcode,
                             const string& city, const string& role) {
    try {
        string hashedPassword = hashPassword(password);
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("INSERT INTO users "
                                                                   "(conjoint, login, password, firstname, lastname, address, postalcode, city, role) "
                                                                   "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);"));
        if (conjoint > 0) {
            pstmt->setInt(1, conjoint);
        } else {
            pstmt->setNull(1, DataType::INTEGER);
        }
        pstmt->setString(2, login);
        pstmt->setString(3, hashedPassword);
        pstmt->setString(4, firstname);
        pstmt->setString(5, lastname);
        pstmt->setString(6, address);
        pstmt->setString(7, postalcode);
        pstmt->setString(8, city);
        pstmt->setString(9, role);
        pstmt->executeUpdate();
        return true;
    } catch (SQLException& e) {
        cerr << "Failed to create user: " << e.what() << endl;
        return false;
    }
}

bool UserQueries::checkLogin(const string& login, const string& password) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT password FROM users WHERE login = ?;"));
        pstmt->setString(1, login);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            string storedHash = static_cast<const char *>(res->getString("password"));
            return storedHash == hashPassword(password);
        }
        return false;
    } catch (SQLException& e) {
        cerr << "Login check failed: " << e.what() << endl;
        return false;
    }
}

string UserQueries::getRole(const string& login) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT role FROM users WHERE login = ?;"));
        pstmt->setString(1, login);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            return static_cast<const char *>(res->getString("role"));
        }
        return "";
    } catch (SQLException& e) {
        cerr << "Failed to retrieve role: " << e.what() << endl;
        return "";
    }
}



string UserQueries::hashPassword(const string& password) {
    unsigned char hash[EVP_MAX_MD_SIZE]; // Buffer for the hash
    unsigned int lengthOfHash = 0;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if(ctx == nullptr) return "";

    if(EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) == 1 &&
       EVP_DigestUpdate(ctx, password.c_str(), password.size()) == 1 &&
       EVP_DigestFinal_ex(ctx, hash, &lengthOfHash) == 1) {
        stringstream ss;
        ss << hex << setfill('0');
        for (unsigned int i = 0; i < lengthOfHash; i++) {
            ss << setw(2) << (int)hash[i];
        }
        EVP_MD_CTX_free(ctx);
        return ss.str();
    }

    EVP_MD_CTX_free(ctx);
    return ""; // Return empty string on failure
}

bool UserQueries::prepareForDeletion(int userId) {
    TransactionQueries transactionQueries(conn);
    if (!transactionQueries.removeUserHistoricEntries(userId)) {
        cerr << "Failed to remove transactions for user ID " << userId << endl;
        return false;
    }


    AccountQueries accountQueries(conn);
    auto accountIds = accountQueries.getUserIdAccounts(userId);
    if (!accountIds.empty()) {

        if (!accountQueries.removeAllAccountsForUser(userId)) {
            cerr << "Failed to remove account ID " << userId << endl;
            return false;
        }
    }

    try {
        // Nullify the conjoint field for any user who has this user set as their conjoint
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("UPDATE users SET conjoint = NULL WHERE conjoint = ?;"));
        pstmt->setInt(1, userId);
        pstmt->executeUpdate();

        return true;
    } catch (SQLException& e) {
        cerr << "Failed to prepare for user deletion: " << e.what() << endl;
        return false;
    }
}

bool UserQueries::removeUser(int userId) {
    if (!prepareForDeletion(userId)) {
        cerr << "Preparation for deletion failed for user ID " << userId << endl;
        return false;
    }

    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("DELETE FROM users WHERE id = ?;"));
        pstmt->setInt(1, userId);
        int rowsAffected = pstmt->executeUpdate();
        return rowsAffected > 0;
    } catch (SQLException& e) {
        cerr << "Failed to remove user: " << e.what() << endl;
        return false;
    }
}




int UserQueries::getUserId(const string& login) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement("SELECT id FROM users WHERE login = ?;"));
        pstmt->setString(1, login);
        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            return res->getInt("id");
        }
    } catch (SQLException& e) {
        cerr << "Failed to get user ID: " << e.what() << endl;
    }
    return -1;
}

unique_ptr<UserModel> UserQueries::getUser(const int &userId) {
    try {
        std::unique_ptr<sql::PreparedStatement> pstmt(conn->prepareStatement("SELECT * FROM users WHERE id = ?"));
        pstmt->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            return make_unique<UserModel>(
                    res->getInt("id"),
                    res->getInt("conjoint"),
                    res->getString("login").c_str(),
                    res->getString("firstname").c_str(),
                    res->getString("lastname").c_str(),
                    res->getString("address").c_str(),
                    res->getString("postalcode").c_str(),
                    res->getString("city").c_str(),
                    res->getString("role").c_str()
            );
        }
    } catch (const sql::SQLException& e) {
        cerr << "SQL Error: " << e.what() << endl;
    }

    return nullptr;
}

bool UserQueries::updateUser(int userId, const map<string, string>& updates) {
    try {
        for (const auto& [field, value] : updates) {
            unique_ptr<PreparedStatement> pstmt;
            if (field == "conjoint" && value == "NULL") {
                pstmt.reset(conn->prepareStatement("UPDATE users SET conjoint = NULL WHERE id = ?;"));
                pstmt->setInt(1, userId);
            } else {
                string query = "UPDATE users SET " + field + " = ? WHERE id = ?;";
                pstmt.reset(conn->prepareStatement(query));
                if (field == "password") {
                    string modifiedValue = hashPassword(value);
                    pstmt->setString(1, modifiedValue);
                } else {
                    pstmt->setString(1, value);
                }
                pstmt->setInt(2, userId);
            }
            pstmt->executeUpdate();
        }
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Failed to update user: " << e.what() << std::endl;
        return false;
    }
}



vector<UserModel> UserQueries::getAllUsers() {
    vector<UserModel> users;
    try {
        unique_ptr<Statement> stmt(conn->createStatement());
        unique_ptr<ResultSet> res(stmt->executeQuery("SELECT * FROM users;"));
        while (res->next()) {
            users.emplace_back(
                    res->getInt("id"),
                    res->getInt("conjoint"),
                    res->getString("login").c_str(),
                    res->getString("firstName").c_str(),
                    res->getString("lastName").c_str(),
                    res->getString("address").c_str(),
                    res->getString("postalcode").c_str(),
                    res->getString("city").c_str(),
                    res->getString("role").c_str()
            );
        }
    } catch (SQLException& e) {
        cerr << "Failed to get all users: " << e.what() << endl;
    }
    return users;
}

int UserQueries::getLastInsertedUserId() {
    try {
        unique_ptr<Statement> stmt(conn->createStatement());
        unique_ptr<ResultSet> res(stmt->executeQuery("SELECT LAST_INSERT_ID() as last_id;"));
        if (res->next()) {
            return res->getInt("last_id");
        }
    } catch (SQLException& e) {
        cerr << "Failed to retrieve last inserted user ID: " << e.what() << endl;
    }
    return -1;
}

string UserQueries::getConjointName(int userId) {
    try {
        unique_ptr<PreparedStatement> pstmt(conn->prepareStatement(
                "SELECT firstname, lastname FROM users WHERE id = ?;"
        ));
        pstmt->setInt(1, userId);

        unique_ptr<ResultSet> res(pstmt->executeQuery());
        if (res->next()) {
            string firstName = res->getString("firstname").c_str();
            string lastName = res->getString("lastname").c_str();
            return firstName + " " + lastName;
        }
    } catch (SQLException& e) {
        cerr << "Failed to retrieve conjoint name: " << e.what() << endl;
        return "N/A";
    }
    return "N/A";
}


