#ifndef USERMANAGEMENTVIEW_H
#define USERMANAGEMENTVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include "../../queries/AccountQueries.h"
#include "AllHistoricView.h"

class UserManagementView : public QWidget {
Q_OBJECT



public:
    explicit UserManagementView(AllHistoricView* allHistoricView, QWidget *parent = nullptr);
    void refreshUserList();


private:
    AllHistoricView* allHistoricView;
    QTableWidget *table;

    QPushButton* removeButton;
    QPushButton* editButton;

    void populateData();

    static QString getAccountInfoForType(int userId, const std::string &accountType, AccountQueries &accountQueries);

    void updateButtons();

    void removeSelectedUser();

    void editSelectedUser();
};

#endif // USERMANAGEMENTVIEW_H

