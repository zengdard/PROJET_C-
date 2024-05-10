#ifndef USEREDITDIALOG_H
#define USEREDITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <map>
#include <QCheckBox>

#include "../../queries/AccountQueries.h"
#include "../../queries/UserQueries.h"

class UserEditDialog : public QDialog {
Q_OBJECT

public:
    explicit UserEditDialog(int userId, QWidget *parent = nullptr);
    ~UserEditDialog() override;

private:
    void setupForm();
    void loadUserData();
    void saveChanges();

    int userId;
    UserQueries *userQueries;
    AccountQueries *accountQueries;  // Declare accountQueries here

    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *firstNameEdit;
    QLineEdit *lastNameEdit;
    QLineEdit *addressEdit;
    QLineEdit *postalcodeEdit;
    QLineEdit *cityEdit;
    QComboBox *roleCombo;
    QComboBox *conjointEdit;
    QPushButton *saveButton;

    QCheckBox* accountTypeCC;
    QCheckBox* accountTypeLC;
    QCheckBox* accountTypePEP;
    QCheckBox* accountTypePB;

    QLineEdit* initialBalanceCC;
    QLineEdit* initialBalanceLC;
    QLineEdit* initialBalancePEP;
    QLineEdit* initialBalancePB;

    void setupAccountControls(QFormLayout *layout);

    void handleAccountUpdates();

    void handleAccountUpdate(const string &accountType, QCheckBox *checkBox, QLineEdit *balanceField);
};

#endif // USEREDITDIALOG_H
