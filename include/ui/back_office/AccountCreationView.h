#ifndef ACCOUNTCREATIONVIEW_H
#define ACCOUNTCREATIONVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QStackedLayout>
#include "../../queries/AccountQueries.h"

class AccountCreationView : public QWidget {
Q_OBJECT

public:
    explicit AccountCreationView(QWidget *parent = nullptr);

signals:
    void userCreation();

private:
    QLineEdit* loginEdit{};
    QLineEdit* passwordEdit{};
    QLineEdit* firstnameEdit{};
    QLineEdit* lastnameEdit{};
    QLineEdit* addressEdit{};
    QLineEdit* postalcodeEdit{};
    QLineEdit* cityEdit{};
    QComboBox* roleCombo{};

    QPushButton* submitButton{};

    QCheckBox *accountTypeCC{};
    QCheckBox *accountTypeLC{};
    QCheckBox *accountTypePEP{};
    QCheckBox *accountTypePB{};

    QHBoxLayout *accountTypesLayout{};
    QHBoxLayout* balanceLayout{};

    QMap<QCheckBox*, QDoubleSpinBox*> checkboxToSpinboxMap;
    QDoubleSpinBox *initialBalanceCC{};
    QDoubleSpinBox *initialBalanceLC{};
    QDoubleSpinBox *initialBalancePEP{};
    QDoubleSpinBox *initialBalancePB{};




    void onCreateButtonClicked();

    QDoubleSpinBox *createInitialBalanceSpinBox();

    void setupForm(QVBoxLayout *layout);

    void setupInitialBalances(QVBoxLayout *layout);

    void setupSubmitButton(QVBoxLayout *layout);

    void connectCheckBoxes();

    void updateSpinBoxVisibility(int state);

    void setupAccountTypeSelection(QVBoxLayout *layout);

    //static string generateUniqueAccountNumber(AccountQueries& accountQueries);

};

#endif //ACCOUNTCREATIONVIEW_H
