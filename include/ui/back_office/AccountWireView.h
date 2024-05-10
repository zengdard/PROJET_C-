#ifndef AccountWireView_H
#define AccountWireView_H

#include "../../../include/ui/common/Card.h"
#include "../../models/AccountModel.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QProgressBar>
#include <QComboBox>

using namespace std;

class AccountWireView : public QWidget {
Q_OBJECT

public:
    explicit AccountWireView(int userId, QWidget *parent = nullptr);

private slots:
    static void updateAccountDetails(QComboBox *accountDropdown, QLabel *accountLabel, QLabel *balanceLabel,
                              QLabel *transactionInfoLabel);

    void onExecuteButtonClicked();

private:
    int userId;

    QHBoxLayout* mainLayout;
    QComboBox* sourceAccountDropdown;
    QComboBox* destinationAccountDropdown;
    QComboBox* sourceUserDropdown;
    QComboBox* destinationUserDropdown;
    QComboBox* userDropdown;
    QLineEdit* initialBalanceCC;
    QLineEdit* initialBalanceLC;
    QLineEdit* initialBalancePEP;
    QLineEdit* accountNumberInput;
    QLabel* balanceLabel;
    QLabel* accountNameLabel;
    QLabel* accountNumberLabel;

    QLabel* transferAmountLabel;
    QLineEdit* transferAmountInput;
    QLabel* feedbackLabel;

    QLabel* sourceInfoLabel, *destinationInfoLabel;
    QLabel* sourceAccountLabel, *destinationAccountLabel;
    QLabel* sourceBalanceLabel, *destinationBalanceLabel;


    Card *sourceCard{};
    Card *destinationCard{};

    QLabel *arrowLabel{};
    QPushButton* executeButton{};
    QLineEdit* amountLineEdit;
    QString accountNumber;

    QProgressBar* progressBar = nullptr;
    QLabel* transactionInfoLabel = nullptr;
    double availableBalance = 10000; // Store the available balance
    vector<AccountModel> accounts;
    int transactionsToday{}; // Store the number of transactions today

    void setupSourceCard();
    void setupDestinationCard();
    void setupArrowLabel();

    void setupCard(QVBoxLayout *layout, QLabel *&infoLabel, QLabel *&accountLabel, QLabel *&balanceLabel,
                   QComboBox *&accountDropdown, const QString &cardTitle);

    void attemptTransfer();

    void performTransfer(const string &sourceAccount, const string &destinationAccount, double amount) const;

    static bool checkTransactionLimits(const string &accountNumber, double amount);

    bool checkTransaction(double amount) const;
    bool performTransaction(const string &sourceAccount, const string &destinationAccount, double amount,
                            double oldSourceBalance, double newSourceBalance, double oldDestBalance,
                            double newDestBalance) const;

    void refreshAccountDetails();

    void loadUsers(QComboBox *userDropdown);

    void loadAccounts(QComboBox *accountDropdown, int usersId);

    void fetchAccountData(QComboBox *accountDropdown, int usersId);
};

#endif // AccountWireView_H

