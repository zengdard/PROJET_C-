#include "../../../include/ui/front_office/HistoryView.h"
#include "../../../include/db/DatabaseManager.h"
#include "../../../include/queries/TransactionQueries.h"

#include <QHeaderView>
#include <QVBoxLayout>
#include <iostream>

HistoryView::HistoryView(int userId, QWidget *parent) : QWidget(parent), userId(userId) {
    setupUI();
    populateData();
}

void HistoryView::setupUI() {
    table = new QTableWidget(0, 8, this);  // 8 columns for each attribute in TransactionModel
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // Resize headers to contents
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);

    QStringList headers = {
            "Date", "Montant", "Numéro de Compte Source", "Ancien Montant Source", "Nouveau Montant Source",
            "Numéro de Compte Destinataire", "Ancien Montant Destinataire", "Nouveau Montant Destinataire"
    };
    table->setHorizontalHeaderLabels(headers);

    // Ensuring the entire header is visible or tooltips are added
    for (int i = 0; i < headers.size(); ++i) {
        table->horizontalHeaderItem(i)->setToolTip(headers[i]);  // Set tooltips
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

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(table);
    setLayout(layout);
}


void HistoryView::populateData() {
    auto *transactionQueries = new TransactionQueries(DatabaseManager::getInstance()->getConnection());
    auto transactions = transactionQueries->getTransactionHistory(userId);

    table->setRowCount(0);  // Clear any existing rows
    for (const auto &transaction : transactions) {
        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(transaction.date)));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(transaction.amount, 'f', 2) + " €"));
        table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(transaction.sourceAccount)));
        table->setItem(row, 3, new QTableWidgetItem(QString::number(transaction.oldBalanceSrc, 'f', 2)+ " €"));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(transaction.newBalanceSrc, 'f', 2)+ " €"));
        table->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(transaction.destinationAccount)));
        table->setItem(row, 6, new QTableWidgetItem(QString::number(transaction.oldBalanceDest, 'f', 2)+ " €"));
        table->setItem(row, 7, new QTableWidgetItem(QString::number(transaction.newBalanceDest, 'f', 2)+ " €"));
    }

    delete transactionQueries;  // Clean up after use
}
