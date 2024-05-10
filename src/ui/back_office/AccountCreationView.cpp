#include <QLineEdit>
#include <QMessageBox>
#include "../../../include/ui/back_office/AccountCreationView.h"
#include "../../../include/queries/UserQueries.h"


AccountCreationView::AccountCreationView(QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    setupForm(mainLayout);
    setupAccountTypeSelection(mainLayout);
    setupInitialBalances(mainLayout);
    setupSubmitButton(mainLayout);
    connectCheckBoxes();
}

void AccountCreationView::setupForm(QVBoxLayout* layout) {
    auto *formLayout = new QFormLayout();
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    formLayout->setLabelAlignment(Qt::AlignRight);

    // User information fields
    loginEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    firstnameEdit = new QLineEdit(this);
    lastnameEdit = new QLineEdit(this);
    addressEdit = new QLineEdit(this);
    postalcodeEdit = new QLineEdit(this);
    cityEdit = new QLineEdit(this);
    roleCombo = new QComboBox(this);
    roleCombo->addItems({"Client", "Stage", "Employé", "Directeur"});

    formLayout->addRow("Nom d'utilisateur", loginEdit);
    formLayout->addRow("Mot de passe", passwordEdit);
    formLayout->addRow("Nom", firstnameEdit);
    formLayout->addRow("Prénom", lastnameEdit);
    formLayout->addRow("Adresse", addressEdit);
    formLayout->addRow("Code Postal", postalcodeEdit);
    formLayout->addRow("Ville", cityEdit);
    formLayout->addRow("Rôle", roleCombo);

    layout->addLayout(formLayout);
}

void AccountCreationView::setupAccountTypeSelection(QVBoxLayout* layout) {
    auto *formLayout = new QFormLayout();
    accountTypesLayout = new QHBoxLayout();
    accountTypeCC = new QCheckBox("Compte Courant", this);
    accountTypeLC = new QCheckBox("Livret C", this);
    accountTypePEP = new QCheckBox("Plan Épargne Logement", this);
    accountTypePB = new QCheckBox("Plan Boursier", this);

    accountTypesLayout->addWidget(accountTypeCC);
    accountTypesLayout->addWidget(accountTypeLC);
    accountTypesLayout->addWidget(accountTypePEP);
    accountTypesLayout->addWidget(accountTypePB);

    formLayout->addRow("Type de Compte", accountTypesLayout);
    layout->addLayout(formLayout);
}

void AccountCreationView::setupInitialBalances(QVBoxLayout* layout) {
    auto *formLayout = new QFormLayout();
    balanceLayout = new QHBoxLayout();
    initialBalanceCC = createInitialBalanceSpinBox();
    initialBalanceLC = createInitialBalanceSpinBox();
    initialBalancePEP = createInitialBalanceSpinBox();
    initialBalancePB = createInitialBalanceSpinBox();

    balanceLayout->addWidget(initialBalanceCC);
    balanceLayout->addWidget(initialBalanceLC);
    balanceLayout->addWidget(initialBalancePEP);
    balanceLayout->addWidget(initialBalancePB);

    formLayout->addRow("Solde Initial", balanceLayout);
    layout->addLayout(formLayout);
}

void AccountCreationView::setupSubmitButton(QVBoxLayout* layout) {
    submitButton = new QPushButton("Créer", this);
    layout->addWidget(submitButton);
    connect(submitButton, &QPushButton::clicked, this, &AccountCreationView::onCreateButtonClicked);
}

QDoubleSpinBox* AccountCreationView::createInitialBalanceSpinBox() {
    auto *spinBox = new QDoubleSpinBox(this);
    spinBox->setMinimum(0);
    spinBox->setMaximum(1000000);
    spinBox->setSingleStep(100);
    spinBox->setVisible(false); // Initially hide the spin box
    return spinBox;
}

void AccountCreationView::connectCheckBoxes() {
    connect(accountTypeCC, &QCheckBox::stateChanged, this, &AccountCreationView::updateSpinBoxVisibility);
    connect(accountTypeLC, &QCheckBox::stateChanged, this, &AccountCreationView::updateSpinBoxVisibility);
    connect(accountTypePEP, &QCheckBox::stateChanged, this, &AccountCreationView::updateSpinBoxVisibility);
    connect(accountTypePB, &QCheckBox::stateChanged, this, &AccountCreationView::updateSpinBoxVisibility);
}

void AccountCreationView::updateSpinBoxVisibility(int state) {
    initialBalanceCC->setVisible(accountTypeCC->isChecked());
    initialBalanceLC->setVisible(accountTypeLC->isChecked());
    initialBalancePEP->setVisible(accountTypePEP->isChecked());
    initialBalancePB->setVisible(accountTypePB->isChecked());
}


void AccountCreationView::onCreateButtonClicked() {
    // Collect user input
    QString login = loginEdit->text();
    QString password = passwordEdit->text();
    QString firstname = firstnameEdit->text();
    QString lastname = lastnameEdit->text();
    QString address = addressEdit->text();
    QString postalcode = postalcodeEdit->text();
    QString city = cityEdit->text();
    QString role = roleCombo->currentText();

    // Interact with UserQueries to create user
    UserQueries userQueries(DatabaseManager::getInstance()->getConnection());
    bool userCreated = userQueries.createUser(0, // assuming '0' or 'NULL' for no conjoint
                                              login.toStdString(), password.toStdString(),
                                              firstname.toStdString(), lastname.toStdString(),
                                              address.toStdString(), postalcode.toStdString(),
                                              city.toStdString(), role.toStdString());

    if (userCreated) {
        int userId = userQueries.getLastInsertedUserId();
        AccountQueries accountQueries(DatabaseManager::getInstance()->getConnection());

        // Iterate over each account type and create account if checked
        if (accountTypeCC->isChecked()) {
            accountQueries.createAccount(userId, "Compte Courant", accountQueries.generateUniqueAccountNumber(),
                                                                 initialBalanceCC->value(), 0);
        }
        if (accountTypeLC->isChecked()) {
            accountQueries.createAccount(userId, "Livret C", accountQueries.generateUniqueAccountNumber(),
                                                                 initialBalanceLC->value(), 0);
        }
        if (accountTypePEP->isChecked()) {
            accountQueries.createAccount(userId, "Plan Épargne Logement", accountQueries.generateUniqueAccountNumber(),
                                                                 initialBalancePEP->value(), 0);
        }
        if (accountTypePB->isChecked()) {
            accountQueries.createAccount(userId, "Plan Boursier", accountQueries.generateUniqueAccountNumber(),
                                                                 initialBalancePB->value(), 0);
        }

        QMessageBox::information(this, "Success", "User and all selected accounts have been successfully created.");

    } else {
        QMessageBox::warning(this, "Error", "Failed to create user.");
    }
    emit userCreation();
}
