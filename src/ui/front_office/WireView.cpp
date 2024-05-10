#include "../../../include/ui/front_office/WireView.h"
#include "../../../include/queries/AccountQueries.h"
#include "../../../include/db/DatabaseManager.h"
#include "../../../include/queries/TransactionQueries.h"
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QComboBox>
#include <QTimer>
#include <QMessageBox>
#include <iostream>
#include <QDate>

WireView::WireView(int userId, QWidget* parent) : QWidget(parent), userId(userId) {
    mainLayout = new QHBoxLayout(this);

    // Setup individual sections with their own layouts
    setupSourceCard();
    setupArrowLabel();
    //setupInputSection();
    setupDestinationCard();

    setLayout(mainLayout);
}

void WireView::setupCard(QVBoxLayout* layout, QLabel*& infoLabel, QLabel*& accountLabel, QLabel*& balanceLabel, QComboBox*& accountDropdown, const QString& cardTitle) {
    // Setup card layout and add labels
    infoLabel = new QLabel(cardTitle, this);
    accountLabel = new QLabel("Numéro de Compte: ", this);
    balanceLabel = new QLabel("Argent Disponible: €0.00", this);

    QString labelStyle = R"(
        QLabel {
            color: #333;
            font-size: 16px;
            font-weight: bold;
            background-color: #f7f7f7;
            border: 1px solid #ccc;
            padding: 8px;
            margin-top: 10px;
        })";

    infoLabel->setStyleSheet(labelStyle);
    accountLabel->setStyleSheet(labelStyle);
    balanceLabel->setStyleSheet(labelStyle);

    infoLabel->setWordWrap(true);
    accountLabel->setWordWrap(true);
    balanceLabel->setWordWrap(true);

    layout->addWidget(infoLabel);
    layout->addWidget(accountLabel);
    layout->addWidget(balanceLabel);

    // Create and add dropdown for account selection
    accountDropdown = new QComboBox(this);
    layout->addWidget(new QLabel("Sélectionner le Type de Compte :", this));
    layout->addWidget(accountDropdown);
}

void WireView::updateAccountDetails(int index, QLabel* accountLabel, QLabel* balanceLabel, QLabel* transactionInfoLabel) {
    if (index < 0 || index >= accounts.size()) { return; }
    const AccountModel& account = accounts[index];

    accountLabel->setText("Numéro de Compte : " + QString::fromStdString(account.accountNumber));
    balanceLabel->setText("Argent Disponible : " + QString::number(account.balance, 'f', 2) + " €");

    if (account.accountType == "Livret C") {
        transactionInfoLabel->setText(QString("Limite de Transaction : %1 sur 1 ou 5 % du montant disponible (%2 €)").arg(account.dailyLimit).arg(account.balance*0.05));
        transactionInfoLabel->setWordWrap(true);
    } else {
        transactionInfoLabel->setText("Pas de limite de transaction pour ce compte.");
    }
}

void WireView::setupSourceCard() {
    sourceCard = new Card(this);
    sourceCard->adjustSize(400, 800);
    QVBoxLayout* sourceLayout = sourceCard->getMainLayout();
    sourceLayout->addStretch();

    // Using setupCard to maintain consistency and ease of updates
    setupCard(sourceLayout, sourceInfoLabel, sourceAccountLabel, sourceBalanceLabel, sourceAccountDropdown, "Information du Compte Source");

    // Transaction information label to provide feedback on transaction limits
    transactionInfoLabel = new QLabel("Les limites de transaction seront affichées ici.");
    sourceLayout->addWidget(transactionInfoLabel);

    fetchAccountData(sourceAccountDropdown);

    if (!sourceAccountDropdown->currentText().isEmpty()) {
        updateAccountDetails(sourceAccountDropdown->currentIndex(), sourceBalanceLabel, sourceBalanceLabel, transactionInfoLabel);
    }

    connect(sourceAccountDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        updateAccountDetails(index, sourceAccountLabel, sourceBalanceLabel, transactionInfoLabel);
    });

    sourceLayout->addStretch();
    mainLayout->addWidget(sourceCard);
}

void WireView::setupDestinationCard() {
    destinationCard = new Card(this);
    destinationCard->adjustSize(400, 800);
    QVBoxLayout* destinationLayout = destinationCard->getMainLayout();
    destinationLayout->addStretch();

    setupCard(destinationLayout, destinationInfoLabel, destinationAccountLabel, destinationBalanceLabel, destinationAccountDropdown, "Détails du Transfert");

    // Adding input for account number which was missing
    accountNumberLabel = new QLabel("Numéro de Compte Destination:");
    accountNumberInput = new QLineEdit();
    accountNumberInput->setPlaceholderText("Entrez le numéro de compte (non fonctionnel pour le moment cela 'est pas relié a l'api)");
    destinationLayout->addWidget(accountNumberLabel);
    destinationLayout->addWidget(accountNumberInput);

    // Transfer amount input setup
    transferAmountLabel = new QLabel("Montant à Transférer (€):");
    transferAmountInput = new QLineEdit();
    transferAmountInput->setValidator(new QDoubleValidator(0, 100000, 2, this));  // Adjust max value as necessary
    transferAmountInput->setPlaceholderText("Entrez le montant");
    destinationLayout->addWidget(transferAmountLabel);
    destinationLayout->addWidget(transferAmountInput);

    // Feedback label for transaction results or errors
    feedbackLabel = new QLabel();
    feedbackLabel->setWordWrap(true);
    destinationLayout->addWidget(feedbackLabel);

    fetchAccountData(destinationAccountDropdown);

    if (!destinationAccountDropdown->currentText().isEmpty()) {
        updateAccountDetails(destinationAccountDropdown->currentIndex(), destinationAccountLabel, destinationBalanceLabel, transactionInfoLabel);
    }

    connect(destinationAccountDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        updateAccountDetails(index, destinationAccountLabel, destinationBalanceLabel, transactionInfoLabel);
    });

    destinationLayout->addStretch();
    mainLayout->addWidget(destinationCard);
}

void WireView::fetchAccountData(QComboBox* accountDropdown) {
    auto *accountQueries = new AccountQueries(DatabaseManager::getInstance()->getConnection());
    accounts = accountQueries->getAccountsByUserId(userId);

    accountDropdown->clear();
    if (!accounts.empty()) {
        for (const auto& account : accounts) {
            QString displayText = QString::fromStdString(account.accountType);
            accountDropdown->addItem(displayText, QVariant(account.id));
        }
    }

    delete accountQueries; // Clean up
}

void WireView::setupArrowLabel() {
    QVBoxLayout* arrowButtonLayout = new QVBoxLayout();  // Create a new vertical layout

    // Setup arrow label
    arrowLabel = new QLabel();
    arrowLabel->setPixmap(QPixmap(":/arrow"));  // Ensure the resource path is correct
    arrowLabel->setAlignment(Qt::AlignCenter);
    arrowButtonLayout->addWidget(arrowLabel);   // Add arrow to the vertical layout

    // Setup execute button
    executeButton = new QPushButton("Execute");
    executeButton->setIcon(QIcon(":/execute"));  // Ensure the resource path is correct
    executeButton->setIconSize(QSize(24, 24));
    executeButton->setStyleSheet(R"(
        QPushButton {
            font-size: 18px;
            font-weight: bold;
            color: #ffffff;
            background-color: #5cb85c;
            border-radius: 10px;
            padding: 10px;
            margin: 10px;
            icon-size: 24px 24px;
        }
        QPushButton:hover {
            background-color: #4cae4c;
        })");

    connect(executeButton, &QPushButton::clicked, this, &WireView::onExecuteButtonClicked);

    arrowButtonLayout->addWidget(executeButton);  // Add button below the arrow in the vertical layout
    arrowButtonLayout->setAlignment(Qt::AlignCenter);  // Align the arrow and button at the center

    // Add the vertical layout containing the arrow and the button to the main layout
    mainLayout->addLayout(arrowButtonLayout);
}

void WireView::onExecuteButtonClicked() {
    double amount = transferAmountInput->text().toDouble();

    auto *accountQueries = new AccountQueries(DatabaseManager::getInstance()->getConnection());

    string prefix = "Numéro de Compte : ";
    // Ensure that the label text is long enough before substring operation
    if (sourceAccountLabel->text().length() <= prefix.length() || destinationAccountLabel->text().length() <= prefix.length()) {
        QMessageBox::warning(this, "Error", "Invalid account number format. Please reselect the accounts.");
        return;
    }

    string sourceAccountNumber = sourceAccountLabel->text().toStdString().substr(prefix.length());
    string destinationAccountNumber = destinationAccountLabel->text().toStdString().substr(prefix.length());

    double availableBalance = accountQueries->getBalance(sourceAccountNumber);
    if (amount > availableBalance) {
        feedbackLabel->setText("Le montant dépasse le solde disponible. Vérifiez les détails et réessayez.");
        QMessageBox::warning(this, "Échec de la transaction", "Le montant dépasse le solde disponible.");
        delete accountQueries;
        return;
    }

    double sourceOldBalance = availableBalance;
    double destinationOldBalance = accountQueries->getBalance(destinationAccountNumber);
    double sourceNewBalance = sourceOldBalance - amount;
    double destinationNewBalance = destinationOldBalance + amount;

    if (sourceAccountNumber == destinationAccountNumber) {
        QMessageBox::warning(this, "Erreur de sélection", "Veuillez sélectionner des comptes source et destination différents.");
        feedbackLabel->setText("Échec de la transaction. Veuillez sélectionner des comptes différents.");
    } else {
        if (performTransaction(sourceAccountNumber, destinationAccountNumber, amount,
                               sourceOldBalance, sourceNewBalance,
                               destinationOldBalance, destinationNewBalance)) {
            feedbackLabel->setText("Transaction réussie !");
            QMessageBox::information(this, "Transaction réussie", "La transaction a été effectuée avec succès.");
            refreshAccountDetails();
        } else {
            feedbackLabel->setText("Échec de la transaction. Vérifiez les détails et réessayez.");
        }
        delete accountQueries; // Clean up
    }
}

bool WireView::checkTransaction(double amount) const {

    auto *accountQueries = new AccountQueries(DatabaseManager::getInstance()->getConnection());
    TransactionQueries transactionQueries(DatabaseManager::getInstance()->getConnection());
    vector<TransactionModel> history = transactionQueries.getTransactionHistory(userId);

    string prefix = "Numéro de Compte : ";
    string sourceAccountNumber = sourceAccountLabel->text().toStdString().substr(prefix.length());
    string sourceAccountType = accountQueries->getAccountType(sourceAccountNumber);

    string destinationNumber = destinationAccountLabel->text().toStdString().substr(prefix.length());
    string destinationAccountType = accountQueries->getAccountType(destinationNumber);

    if (sourceAccountType == "Livret C") {
        string sourceBalance = accountQueries->getAccountNumber(userId, "Livret C");

        double initialBalanceCCValue = accountQueries->getBalance(sourceBalance);
        double fivePercentOfSourceBalance = initialBalanceCCValue * 0.05;

        string lastTransactionDateString = accountQueries->getLastTransaction(sourceAccountNumber);
        QDate lastTransactionDate = QDate::fromString(QString::fromStdString(lastTransactionDateString), "yyyy-MM-dd");

        // Check if there was a transaction today and the requested amount is more than allowed
        if (QDate::currentDate() == lastTransactionDate) {
            QMessageBox::warning(nullptr, "Transaction Error", "Only one transaction per day is allowed for Livret C accounts.");
            delete accountQueries;
            return false;
        }
        if (amount > fivePercentOfSourceBalance || amount > 10000) {
            QMessageBox::warning(nullptr, "Transaction Error", "Withdrawal from Livret C exceeds allowed percentage or exceeds the €10,000 limit.");
            delete accountQueries;
            return false;
        }
    }

    // Conditions for deposits into Livret C as the destination account
    if (destinationAccountType == "Livret C") {
        string accountn = accountQueries->getAccountNumber(userId, "Livret C");
        double initialBalanceCCValue = accountQueries->getBalance(accountn);
        double total = initialBalanceCCValue +  amount;

        if (amount < 1 || total > 10000) {
            QMessageBox::warning(nullptr, "Transaction Error", "Deposits into Livret C must be at least 1€ and don't exceed a 10000€ total balance.");
            delete accountQueries;
            return false;
        }

    } else if (sourceAccountType == "Plan Épargne Logement" || ((destinationAccountType == "Plan Épargne Logement" && amount < 50))) {
        QMessageBox::warning(nullptr, "Transaction Error", "Deposits into Plan Épargne Logement must be at least 1€ and you can't withdraw.");
        return false;
    } else if (sourceAccountType == "Plan Boursier"){
        double initialBalanceCCValue = accountQueries->getBalance(accountQueries->getAccountNumber(userId, "Plan Boursier"));
        if (initialBalanceCCValue != amount){
            QMessageBox::warning(nullptr, "Transaction Error", "You must withdraw the entire balance.");
            return false;
        }

     }

    return true;
}

bool WireView::performTransaction(const string& sourceAccount, const string& destinationAccount, double amount,
                                      double oldSourceBalance, double newSourceBalance, double oldDestBalance, double newDestBalance) const {

    auto *transactionQueries = new TransactionQueries(DatabaseManager::getInstance()->getConnection());
    auto *accountQueries = new AccountQueries(DatabaseManager::getInstance()->getConnection());

    bool transactionSuccess = checkTransaction(amount);
    if (!transactionSuccess) {
        return false;
    }


    bool success = accountQueries->updateAccountBalance(sourceAccount, newSourceBalance) &&
                   accountQueries->updateAccountBalance(destinationAccount, newDestBalance) &&
                   transactionQueries->recordTransaction(userId, sourceAccount, destinationAccount,
                                                         amount, oldSourceBalance,
                                                         newSourceBalance, oldDestBalance,
                                                         newDestBalance);

    delete transactionQueries;
    delete accountQueries;
    return success;

}

void WireView::refreshAccountDetails() {
    // Save current selections to restore them later
    int sourceIndex = sourceAccountDropdown->currentIndex();
    QVariant sourceAccountId = sourceAccountDropdown->currentData();
    int destinationIndex = destinationAccountDropdown->currentIndex();
    QVariant destinationAccountId = destinationAccountDropdown->currentData();

    // Fetch the latest account data
    fetchAccountData(sourceAccountDropdown);
    fetchAccountData(destinationAccountDropdown);

    // Restore the previous selections if possible
    if (sourceAccountId.isValid()) {
        int newIndex = sourceAccountDropdown->findData(sourceAccountId);
        if (newIndex != -1) {
            sourceAccountDropdown->setCurrentIndex(newIndex);
        } else if (sourceIndex < sourceAccountDropdown->count()) {
            sourceAccountDropdown->setCurrentIndex(sourceIndex);
        }
    }

    if (destinationAccountId.isValid()) {
        int newIndex = destinationAccountDropdown->findData(destinationAccountId);
        if (newIndex != -1) {
            destinationAccountDropdown->setCurrentIndex(newIndex);
        } else if (destinationIndex < destinationAccountDropdown->count()) {
            destinationAccountDropdown->setCurrentIndex(destinationIndex);
        }
    }

    // Update account details displays based on new selections
    updateAccountDetails(sourceAccountDropdown->currentIndex(), sourceAccountLabel, sourceBalanceLabel, transactionInfoLabel);
    updateAccountDetails(destinationAccountDropdown->currentIndex(), destinationAccountLabel, destinationBalanceLabel, transactionInfoLabel);
}

