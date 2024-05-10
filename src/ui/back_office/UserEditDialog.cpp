#include "../../../include/ui/back_office/UserEditDialog.h"

#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>

UserEditDialog::UserEditDialog(int userId, QWidget *parent) :
        QDialog(parent),
        userId(userId),
        userQueries(new UserQueries(DatabaseManager::getInstance()->getConnection())),
        accountQueries(new AccountQueries(DatabaseManager::getInstance()->getConnection())) {
    setWindowTitle("Edit User");
    setupForm();
    loadUserData();


    connect(accountTypeCC, &QCheckBox::stateChanged, this, [=]() {
        initialBalanceCC->setVisible(accountTypeCC->isChecked());
    });
    connect(accountTypeLC, &QCheckBox::stateChanged, this, [=]() {
        initialBalanceLC->setVisible(accountTypeLC->isChecked());
    });
    connect(accountTypePEP, &QCheckBox::stateChanged, this, [=]() {
        initialBalancePEP->setVisible(accountTypePEP->isChecked());
    });
    connect(accountTypePB, &QCheckBox::stateChanged, this, [=]() {
        initialBalancePB->setVisible(accountTypePB->isChecked());
    });
}

UserEditDialog::~UserEditDialog() {
    delete userQueries;
    delete accountQueries;
}

void UserEditDialog::setupForm() {
    QFormLayout *layout = new QFormLayout(this);  // Use a pointer that's accessible within the class scope if needed elsewhere

    conjointEdit = new QComboBox(this);
    conjointEdit->addItem("None", QVariant(-1)); // Option for no conjoint
    auto allUsers = userQueries->getAllUsers();
    for (const auto &user : allUsers) {
        QString userDisplayName = QString::fromStdString(user.firstName + " " + user.lastName);
        conjointEdit->addItem(userDisplayName, QVariant(user.id));
    }

    loginEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    firstNameEdit = new QLineEdit(this);
    lastNameEdit = new QLineEdit(this);
    addressEdit = new QLineEdit(this);
    postalcodeEdit = new QLineEdit(this);
    cityEdit = new QLineEdit(this);
    roleCombo = new QComboBox(this);
    roleCombo->addItems({"Client", "Stage", "Employé", "Directeur"});

    // Setup account type checkboxes and balance fields
    setupAccountControls(layout);

    saveButton = new QPushButton("Save Changes", this);
    connect(saveButton, &QPushButton::clicked, this, &UserEditDialog::saveChanges);

    layout->addRow(new QLabel("Conjoint:"), conjointEdit);
    layout->addRow(new QLabel("Login:"), loginEdit);
    layout->addRow(new QLabel("Password (leave blank if unchanged):"), passwordEdit);
    layout->addRow(new QLabel("First Name:"), firstNameEdit);
    layout->addRow(new QLabel("Last Name:"), lastNameEdit);
    layout->addRow(new QLabel("Address:"), addressEdit);
    layout->addRow(new QLabel("Postal Code:"), postalcodeEdit);
    layout->addRow(new QLabel("City:"), cityEdit);
    layout->addRow(new QLabel("Role:"), roleCombo);
    layout->addRow(saveButton);

    this->setLayout(layout);
}

void UserEditDialog::setupAccountControls(QFormLayout* layout) {
    accountTypeCC = new QCheckBox("Compte Courant", this);
    accountTypeLC = new QCheckBox("Livret C", this);
    accountTypePEP = new QCheckBox("Plan Épargne Logement", this);
    accountTypePB = new QCheckBox("Plan Boursier", this);

    initialBalanceCC = new QLineEdit(this);
    initialBalanceLC = new QLineEdit(this);
    initialBalancePEP = new QLineEdit(this);
    initialBalancePB = new QLineEdit(this);

    layout->addRow(accountTypeCC, initialBalanceCC);
    layout->addRow(accountTypeLC, initialBalanceLC);
    layout->addRow(accountTypePEP, initialBalancePEP);
    layout->addRow(accountTypePB, initialBalancePB);
}

void UserEditDialog::loadUserData() {
    auto user = userQueries->getUser(userId);
    if (user) {
        loginEdit->setText(QString::fromStdString(user->login));
        firstNameEdit->setText(QString::fromStdString(user->firstName));
        lastNameEdit->setText(QString::fromStdString(user->lastName));
        addressEdit->setText(QString::fromStdString(user->address));
        postalcodeEdit->setText(QString::fromStdString(user->postalcode));
        cityEdit->setText(QString::fromStdString(user->city));
        roleCombo->setCurrentText(QString::fromStdString(user->role));
        conjointEdit->setCurrentIndex(conjointEdit->findData(user->conjoint));

        auto accounts = accountQueries->getAccountsByUserId(userId);
        for (const auto& account : accounts) {
            if (account.accountType == "Compte Courant") {
                accountTypeCC->setChecked(true);
                initialBalanceCC->setText(QString::number(account.balance));
                initialBalanceCC->setVisible(true);
            } else if (account.accountType == "Livret C") {
                accountTypeLC->setChecked(true);
                initialBalanceLC->setText(QString::number(account.balance));
                initialBalanceLC->setVisible(true);
            } else if (account.accountType == "Plan Épargne Logement") {
                accountTypePEP->setChecked(true);
                initialBalancePEP->setText(QString::number(account.balance));
                initialBalancePEP->setVisible(true);
            } else if (account.accountType == "Plan Boursier") {
                accountTypePB->setChecked(true);
                initialBalancePB->setText(QString::number(account.balance));
                initialBalancePB->setVisible(true);
            }
        }
    } else {
        QMessageBox::critical(this, "Error", "User data could not be loaded.");
        reject();
    }
}

void UserEditDialog::saveChanges() {
    map<string, string> updates = {
            {"login", loginEdit->text().toStdString()},
            {"firstName", firstNameEdit->text().toStdString()},
            {"lastName", lastNameEdit->text().toStdString()},
            {"address", addressEdit->text().toStdString()},
            {"postalcode", postalcodeEdit->text().toStdString()},
            {"city", cityEdit->text().toStdString()},
            {"role", roleCombo->currentText().toStdString()},
    };

    if (!passwordEdit->text().isEmpty()) {
        updates["password"] = passwordEdit->text().toStdString();
    }

    QVariant conjointData = conjointEdit->currentData();
    if (conjointData.isValid() && conjointData.toInt() != -1) {
        updates["conjoint"] = conjointData.toString().toStdString();
    } else {
        updates["conjoint"] = "NULL"; // Properly handled in updateUser now
    }

    if (userQueries->updateUser(userId, updates)) {
        handleAccountUpdates();
        QMessageBox::information(this, "Success", "User updated successfully.");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Failed to update user.");
    }
}


void UserEditDialog::handleAccountUpdates() {
    handleAccountUpdate("Compte Courant", accountTypeCC, initialBalanceCC);
    handleAccountUpdate("Livret C", accountTypeLC, initialBalanceLC);
    handleAccountUpdate("Plan Épargne Logement", accountTypePEP, initialBalancePEP);
    handleAccountUpdate("Plan Boursier", accountTypePB, initialBalancePB);
}

void UserEditDialog::handleAccountUpdate(const string& accountType, QCheckBox *checkBox, QLineEdit *balanceField) {
    if (checkBox->isChecked()) {
        double balance = balanceField->text().toDouble();
        if (!accountQueries->accountExists(userId, accountType)) {
            // Create the account if it doesn't exist
            string accountNumber = accountQueries->generateUniqueAccountNumber();
            accountQueries->createAccount(userId, accountType, accountNumber, balance, 0);
        } else {
            // Update the existing account's balance
            accountQueries->updateAccountBalance(userId, accountType, balance);
        }
    } else {
        if (accountQueries->accountExists(userId, accountType)) {
            // Remove the account if unchecked
            accountQueries->removeAccountByType(userId, accountType);
        }
    }
}

