#ifndef USERMODEL_H
#define USERMODEL_H

#include <string>

using namespace std;

class UserModel {
public:
    int id;
    int conjoint;
    string login;
    string firstName;
    string lastName;
    string address;
    string postalcode;
    string city;
    string role;

    

    // Constructor to initialize the user model
    UserModel(int id, const int& conjoint, const string& login, const string& firstName,
              const string& lastName, const string& address, const string& postalcode, const string& city, const string& role)
            : id(id), conjoint(conjoint), login(login), firstName(firstName), lastName(lastName),
              address(address), postalcode(postalcode), city(city), role(role) {}
};

#endif // USERMODEL_H
