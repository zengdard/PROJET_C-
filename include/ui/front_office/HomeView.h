#ifndef HOMEVIEW_H
#define HOMEVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "../../../include/db/DatabaseManager.h"
#include "../../../include/queries/AccountQueries.h"

class HomeView : public QWidget {
Q_OBJECT

public:
    explicit HomeView(int userId, QWidget *parent = nullptr);
    void setupAccountCard();
    void fetchAndDisplayAccountData();

private:
    int userId;
    QVBoxLayout* mainLayout;
    QLabel* accountTypeLabel;
    QLabel* accountNumberLabel;
    QLabel* accountBalanceLabel;
    QLabel* lastTransactionDateLabel;

    void createAccountCard(const AccountModel &account);
};

#endif // HOMEVIEW_H
