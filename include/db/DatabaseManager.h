#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <mariadb/conncpp.hpp>
#include <memory>

using namespace std;
using namespace sql;

class DatabaseManager {
private:
    static unique_ptr<DatabaseManager> instance;
    unique_ptr<Connection> con; // Keep using smart pointers for resource management

    // Constructor should be private for singleton

public:
    static DatabaseManager* getInstance();
    void connectToDatabase();
    Connection* getConnection();
    void initializeDatabase();

    DatabaseManager();
};

#endif // DATABASEMANAGER_H
