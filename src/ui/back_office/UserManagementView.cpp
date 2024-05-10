#include "../../../include/ui/back_office/UserManagementView.h"
#include "../../../include/db/DatabaseManager.h"
#include "../../../include/queries/UserQueries.h"
#include "../../../include/ui/back_office/UserEditDialog.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

UserManagementView::UserManagementView(AllHistoricView* allHistoricView, QWidget *parent)
        : QWidget(parent), allHistoricView(allHistoricView) {

    // Setup UI or other initialization
    table = new QTableWidget(0, 13, this);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);

    // Define the headers based on the user data fields you wish to display
    QStringList headers = {"ID", "Conjoint", "Login", "Nom", "Prénom", "Address", "Code Postal", "Ville",  "Rôle",
                           "Compte Courant", "Livret C", "Plan Épargne Logement", "Plan Boursier"};
    table->setHorizontalHeaderLabels(headers);

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

    removeButton = new QPushButton("Remove", this);
    editButton = new QPushButton("Edit", this);
    removeButton->setEnabled(false);
    editButton->setEnabled(false);

    // Connect signals
    connect(table, &QTableWidget::itemSelectionChanged, this, &UserManagementView::updateButtons);
    connect(removeButton, &QPushButton::clicked, this, &UserManagementView::removeSelectedUser);
    connect(editButton, &QPushButton::clicked, this, &UserManagementView::editSelectedUser);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(removeButton);
    layout->addWidget(editButton);

    layout->addWidget(table);
    setLayout(layout);

    populateData();
}

void UserManagementView::populateData() {
    UserQueries userQueries(DatabaseManager::getInstance()->getConnection());
    AccountQueries accountQueries(DatabaseManager::getInstance()->getConnection());
    auto users = userQueries.getAllUsers();

    int row = 0;
    for (const auto& user : users) {
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(QString::number(user.id)));
        if (user.conjoint > 0) {
            table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(userQueries.getConjointName(user.conjoint))));
        } else {
            table->setItem(row, 1, new QTableWidgetItem("N/A"));
        }

        table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(user.login)));
        table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(user.firstName)));
        table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(user.lastName)));
        table->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(user.address)));
        table->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(user.postalcode)));
        table->setItem(row, 7, new QTableWidgetItem(QString::fromStdString(user.city)));
        table->setItem(row, 8, new QTableWidgetItem(QString::fromStdString(user.role)));

        table->setItem(row, 9, new QTableWidgetItem(getAccountInfoForType(user.id, "Compte Courant", accountQueries)));
        table->setItem(row, 10, new QTableWidgetItem(getAccountInfoForType(user.id, "Livret C", accountQueries)));
        table->setItem(row, 11, new QTableWidgetItem(getAccountInfoForType(user.id, "Plan Épargne Logement", accountQueries)));
        table->setItem(row, 12, new QTableWidgetItem(getAccountInfoForType(user.id, "Plan Boursier", accountQueries)));
        ++row;
    }

    table->resizeColumnsToContents();
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

QString UserManagementView::getAccountInfoForType(int userId, const std::string& accountType, AccountQueries& accountQueries) {
    auto accountNumber = accountQueries.getAccountNumber(userId, accountType);
    if (!accountNumber.empty()) {
        auto balance = accountQueries.getBalance(accountNumber);
        return QString::number(balance) + " €";
    }
    return "N/A";
}

void UserManagementView::updateButtons() {
    int selectedCount = table->selectionModel()->selectedRows().count();
    removeButton->setEnabled(selectedCount > 0);
    editButton->setEnabled(selectedCount > 0);
}

void UserManagementView::removeSelectedUser() {
    int row = table->currentRow();
    if (row != -1) {
        int userId = table->item(row, 0)->text().toInt();
        UserQueries userQueries(DatabaseManager::getInstance()->getConnection());
        if (userQueries.removeUser(userId)) {
            table->removeRow(row);
            refreshUserList();
            QMessageBox::information(this, "Success", "User removed successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to remove user.");
        }
    }
}

void UserManagementView::editSelectedUser() {
    int row = table->currentRow();
    if (row != -1) {
        int userId = table->item(row, 0)->text().toInt();
        // Open an edit dialog
        UserEditDialog *editDialog = new UserEditDialog(userId, this);
        if (editDialog->exec() == QDialog::Accepted) {
            refreshUserList();
        }
        delete editDialog;
    }
}

void UserManagementView::refreshUserList() {
    table->clearContents();
    table->setRowCount(0);
    populateData();

    if (allHistoricView) {
        allHistoricView->refreshHistoricData();  // Refresh historic data view
    }
}
