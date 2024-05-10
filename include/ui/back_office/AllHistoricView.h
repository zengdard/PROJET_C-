#ifndef HISTORICVIEW_H
#define HISTORICVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>

class AllHistoricView : public QWidget {
Q_OBJECT

public:
    explicit AllHistoricView(int userId, QWidget *parent = nullptr);
    void refreshHistoricData();

private:
    QTableWidget *table;
    QComboBox* userDropdown;
    QComboBox* accountDropdown;

    int userId;
    void setupUI();
    void populateData(); // Function to fill the table with data

    void setupTable();

    void loadUsers();

    void loadAccounts(int userIndex);

    void onUserSelected(int index);

    void refreshView();
};

#endif //HISTORICVIEW_H

