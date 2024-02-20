#include <iostream>
#include "bankingApp/bankingApp.h"
#include "frontOffice/user.h"
#include "backOffice/backOffice.h"

int main() {
    BankingApp app;
    std::cout << "Choissisez le lieu de votre enregistrement" << std::endl;
    std::cout << "1. Front Office" << std::endl;
    std::cout << "2. Back Office" << std::endl;

    int choice = 0;
    std::cin >> choice;
    std::system("clear");
    // Validate the input
    while (choice != 1 && choice != 2) {
        std::cout << "Veuillez entrer 1 pour le Front Office et 2 pour le Back Office: ";
        std::cin >> choice;
    }

    if(choice == 1) {
        User user(app.getConnection());

        std::string login, pwd;
        std::cout << "LOGIN";
        std::cin >> login;
        std::cout << "PWD";
        std::cin >> pwd;

        if (user.authenticate(login, pwd)) {
            std::cout << "Loged" << std::endl;
            user.listAccounts();
            while (true) {


                std::cout << "1 => Créditer Compte" << std::endl;
                std::cout << "2 => Débiter le comtpe" << std::endl;
                std::cout << "3 => Afficher les comptes" << std::endl;
                std::cout << "4 => Sortir" << std::endl;
                std::cout << "Choix =>";

                std::cin >> choice;


                int somme;
                switch(choice) {
                    case 1: {
                        std::cout << "ID du compte à Créditer: " << std::endl;
                        std::cin >> choice;
                        std::cout << "somme à Créditer: " << std::endl;
                        std::cin >> somme;

                        user.updateAccountBalance(somme, choice);
                        break;
                    }
                    case 2: {
                        std::cout << "ID du compte à Débiter: " << std::endl;
                        std::cin >> choice;
                        std::cout << "somme à Débiter: " << std::endl;
                        std::cin >> somme;
                        user.updateAccountBalance(-somme, choice);
                        break;
                    }
                    case 3: {
                        std::vector<User::Account> accounts = user.listAccounts();

                        // If you need to do something with the list of accounts, you can iterate over them here
                        for (const auto& account : accounts) {

                            std::cout << "Compte: " << account.name << std::endl;
                            std::cout << "Solde: " << account.balance << std::endl;
                            std::cout << "ID: " << account.id << std::endl;

                            std::cout << "____________________" << std::endl;
                        }
                        break;
                    }
                    case 4: {
                        main();
                        break;
                    }
                }


            }

        }
    }
    else {
        BackOffice back_office(app.getConnection());

        std::string login, pwd;
        std::cout << "Login: ";
        std::cin >> login;
        std::cout << "PWD: ";
        std::cin >> pwd;


        if(back_office.authenticate(login,pwd)) {
            while(true) {
                std::cout << "1 => Créer un compte" << std::endl;
                std::cout << "2 => Créer et associer un compte bancaire" << std::endl;
                std::cout << "3 => Lister les utilisateurs" << std::endl;
                std::cout << "4 => Sortir" << std::endl;
                std::cout << "Entrez =>";

                std::cin >> choice;
                std::string nom, prenom, loginUser, pwdUser, accountName;
                double initialBalance;
                switch(choice) {
                    case 1: {
                        std::cout << "Prénom: ";
                        std::cin >> prenom;
                        std::cout << "Nom: ";
                        std::cin >> nom;
                        std::cout << "Login: ";
                        std::cin >> loginUser;
                        std::cout << "PWD: ";
                        std::cin >> pwdUser;
                        back_office.createUser(nom,prenom,loginUser,pwdUser);
                        break;
                    }
                    case 2: {
                        std::cout << "Veuillez entrer le login de l'utilisateur concerné: ";
                        std::cin >> loginUser;
                        switch (back_office.checkAccountTypes(loginUser)) {
                            case 0: {
                                int choix;
                                std::cout << "Type de compte ? (1 pour Livret Epargne, 2 pour Livret A) :";
                                std::cin >> choix;
                                choix == 1 ? accountName = "Livret Epargne" : accountName = "Livret A";
                                std::cout << "somme 0: ";
                                std::cin >> initialBalance;
                                back_office.createAndAssociateAccount(loginUser,accountName,initialBalance);
                                break;
                            }
                            case 1: {
                                std::cout << "l'Utilisateur possède déjà un Livret Epargne, Création de Livret A" << std::endl;
                                accountName = "Livret A";
                                std::cout << "Veuillez entrer un somme initial: ";
                                std::cin >> initialBalance;
                                back_office.createAndAssociateAccount(loginUser,accountName,initialBalance);
                                break;
                            }
                            case 2: {
                                std::cout << "l'Utilisateur possède déjà un Livret A, Création d'un Livret Epargne" << std::endl;
                                accountName = "Livret Epargne";
                                std::cout << "Veuillez entrer un somme initial: ";
                                std::cin >> initialBalance;
                                back_office.createAndAssociateAccount(loginUser,accountName,initialBalance);
                                break;
                            }
                            case 3: {
                                std::cout << "l'Utilisateur possède déjà les deux produits." << std::endl;
                                break;
                            }
                        }
                    }
                    case 3: {
                        std::vector<BackOffice::User> users = back_office.listUsers();
                        for (const auto& user : users) {
                            std::cout << "ID: " << user.id << ", Login: " << user.login << std::endl;
                        }
                        break;
                    }
                    case 4: {
                        main();
                        break;
                    }
                }
            }
        }
    }

    return 0;
}
