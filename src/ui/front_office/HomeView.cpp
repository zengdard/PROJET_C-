#include "../../../include/ui/common/Card.h"
#include "../../../include/queries/AccountQueries.h"
#include "../../../include/db/DatabaseManager.h"
#include "../../../include/ui/front_office/HomeView.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QString>

HomeView::HomeView(int userId, QWidget* parent) : QWidget(parent), userId(userId) {
    mainLayout = new QVBoxLayout(this);
    fetchAndDisplayAccountData();
    setLayout(mainLayout);
}

void HomeView::fetchAndDisplayAccountData() {
    auto conn = DatabaseManager::getInstance()->getConnection();
    AccountQueries accountQueries(conn);
    auto accounts = accountQueries.getAccountsByUserId(userId);

    if (accounts.empty()) {
        Card* accountCard = new Card(this);
        QVBoxLayout* cardLayout = accountCard->getMainLayout();

        QLabel* noAccountLabel = new QLabel("Aucun compte disponible", this);
        noAccountLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
        noAccountLabel->setAlignment(Qt::AlignCenter);

        cardLayout->addWidget(noAccountLabel);
        mainLayout->addWidget(accountCard);
    } else {
        for (const auto& account : accounts) {
            createAccountCard(account);
        }
    }
}

void HomeView::createAccountCard(const AccountModel& account) {
    Card* accountCard = new Card(this);
    QVBoxLayout* cardLayout = accountCard->getMainLayout();

    QLabel* accountTypeLabel = new QLabel("Type de Compte : " + QString::fromStdString(account.accountType), this);
    QLabel* accountNumberLabel = new QLabel("Numéro de Compte : " + QString::fromStdString(account.accountNumber), this);
    QLabel* accountBalanceLabel = new QLabel("Argent Disponible : " + QString::number(account.balance, 'f', 2) + " €", this);
    QLabel* lastTransactionDateLabel = new QLabel("Date de la Dernière Transaction : " + QString::fromStdString(account.lastTransactionDate), this);

    accountTypeLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    accountNumberLabel->setStyleSheet("font-size: 16px;");
    accountBalanceLabel->setStyleSheet("font-size: 16px;");
    lastTransactionDateLabel->setStyleSheet("font-size: 16px;");

    cardLayout->addWidget(accountTypeLabel);
    cardLayout->addWidget(accountNumberLabel);
    cardLayout->addWidget(accountBalanceLabel);
    cardLayout->addWidget(lastTransactionDateLabel);

    if(account.accountType == "Livret C") {
        QLabel* dailyLimitLabel = new QLabel("Limite Journalière : " +
                                             QString::number(account.dailyLimit) +
                                             " sur 1 ou 5% de l'argent disponible soit " +
                                             QString::number(account.balance*0.05, 'f', 2) + " €", this);
        dailyLimitLabel->setStyleSheet("font-size: 16px;");
        cardLayout->addWidget(dailyLimitLabel);
    }

    mainLayout->addWidget(accountCard);
}