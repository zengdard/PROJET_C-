#include "../../../include/ui/back_office/AllHistoricView.h"

#include "../../../include/db/DatabaseManager.h"
#include "../../../include/queries/TransactionQueries.h"
#include "../../../include/queries/UserQueries.h"
#include "../../../include/queries/AccountQueries.h"

#include <QHeaderView>
#include <QVBoxLayout>
#include <iostream>


AllHistoricView::AllHistoricView(int userId, QWidget *parent) : QWidget(parent), userId(userId) {
    setupUI();
    loadUsers();
    populateData();
}

void AllHistoricView::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this); // Main layout for the view

    // User selection
    QHBoxLayout *userLayout = new QHBoxLayout();
    userDropdown = new QComboBox(this);
    userDropdown->addItem("All Users", QVariant(-1));
    userLayout->addWidget(new QLabel("Select User:", this));
    userLayout->addWidget(userDropdown);
    layout->addLayout(userLayout);

    // Account selection
    QHBoxLayout *accountLayout = new QHBoxLayout();
    accountDropdown = new QComboBox(this);
    accountDropdown->addItem("All Accounts", QVariant(-1));
    accountLayout->addWidget(new QLabel("Select Account:", this));
    accountLayout->addWidget(accountDropdown);
    layout->addLayout(accountLayout);

    // Setup table
    setupTable();
    layout->addWidget(table);

    // Connect dropdowns to their slots
    connect(userDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AllHistoricView::onUserSelected);
    connect(accountDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AllHistoricView::refreshView);

    setLayout(layout);
}

void AllHistoricView::setupTable() {
    table = new QTableWidget(0, 9, this);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    QStringList headers = {
            "Date", "Utilisateur", "Montant", "Numéro de Compte Source", "Ancien Montant Source",
            "Nouveau Montant Source", "Numéro de Compte Destinataire", "Ancien Montant Destinataire",
            "Nouveau Montant Destinataire"
    };
    table->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < headers.size(); ++i) {
        table->horizontalHeaderItem(i)->setToolTip(headers[i]);
    }

    QFont font = table->horizontalHeader()->font();
    font.setBold(true);
    table->horizontalHeader()->setFont(font);
    table->setStyleSheet(R"(
        QTableWidget {
            border: none;
            selection-background-color: #A2D2FF;
        }
        QHeaderView::section {
            background-color: #F0F0F0;
            padding: 4px;
            border: 1px solid #e0e0e0;
            font-size: 14px;
        }
        QTableWidget::item {
            padding: 4px;
        }
    )");
}

void AllHistoricView::loadUsers() {
    auto *userQueries = new UserQueries(DatabaseManager::getInstance()->getConnection());
    auto users = userQueries->getAllUsers();
    for (const auto& user : users) {
        userDropdown->addItem(QString("%1 %2").arg(QString::fromStdString(user.firstName), QString::fromStdString(user.lastName)),
                              QVariant(user.id));
    }
    delete userQueries; // Release resources
}

void AllHistoricView::loadAccounts(int userId) {
    accountDropdown->clear();
    accountDropdown->addItem("All Accounts", QVariant(-1)); // Default option for all accounts
    auto *accountQueries = new AccountQueries(DatabaseManager::getInstance()->getConnection());
    auto accounts = accountQueries->getAccountsByUserId(userId);
    for (const auto& account : accounts) {
        QVariant data = QVariant::fromValue(QString::fromStdString(account.accountType) + "|" + QString::fromStdString(account.accountNumber));
        accountDropdown->addItem(QString("%1 - %2").arg(QString::fromStdString(account.accountType), QString::fromStdString(account.accountNumber)), data);
    }
}


// getConjointName
void AllHistoricView::populateData() {
    auto *transactionQueries = new TransactionQueries(DatabaseManager::getInstance()->getConnection());
    auto transactions = transactionQueries->getAllTransactionHistory();

    table->setRowCount(0);
    for (const auto &transaction : transactions) {
        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(transaction.date)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(transaction.fullName)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(transaction.amount, 'f', 2) + " €"));
        table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(transaction.sourceAccount)));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(transaction.oldBalanceSrc, 'f', 2) + " €"));
        table->setItem(row, 5, new QTableWidgetItem(QString::number(transaction.newBalanceSrc, 'f', 2) + " €"));
        table->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(transaction.destinationAccount)));
        table->setItem(row, 7, new QTableWidgetItem(QString::number(transaction.oldBalanceDest, 'f', 2) + " €"));
        table->setItem(row, 8, new QTableWidgetItem(QString::number(transaction.newBalanceDest, 'f', 2) + " €"));
    }
}
void AllHistoricView::onUserSelected(int index) {
    int selectedUserId = userDropdown->itemData(index).toInt();
    loadAccounts(selectedUserId);
    refreshView(); // Refresh the table view based on the selected user
}

void AllHistoricView::refreshView() {
    auto *transactionQueries = new TransactionQueries(DatabaseManager::getInstance()->getConnection());
    QString accountData = accountDropdown->currentData().toString();
    int userId = userDropdown->currentData().toInt();
    int accountId = accountDropdown->currentData().toInt();

    vector<TransactionModel> transactions;
    if (userId == -1) {  // If 'All Users' is selected
        transactions = transactionQueries->getAllTransactionHistory();
    } else if (accountData == "-1") {
        // Fetch all transactions for the selected user
        transactions = transactionQueries->getTransactionHistory(userId);
    } else {
        QStringList accountDetails = accountData.split("|");
        if (accountDetails.size() == 2) {
            QString accountNumber = accountDetails[1];
            transactions = transactionQueries->getTransactionsByAccountId(accountNumber.toStdString());
        }
    }

    table->setRowCount(0);
    for (const auto &transaction : transactions) {
        int row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(transaction.date)));
        table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(transaction.fullName)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(transaction.amount, 'f', 2) + " €"));
        table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(transaction.sourceAccount)));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(transaction.oldBalanceSrc, 'f', 2) + " €"));
        table->setItem(row, 5, new QTableWidgetItem(QString::number(transaction.newBalanceSrc, 'f', 2) + " €"));
        table->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(transaction.destinationAccount)));
        table->setItem(row, 7, new QTableWidgetItem(QString::number(transaction.oldBalanceDest, 'f', 2) + " €"));
        table->setItem(row, 8, new QTableWidgetItem(QString::number(transaction.newBalanceDest, 'f', 2) + " €"));
    }
}

void AllHistoricView::refreshHistoricData() {
    populateData(); // Assuming populateData fetches and displays the latest data
}

