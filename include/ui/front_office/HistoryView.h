#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include <QWidget>
#include <QTableWidget>

class HistoryView : public QWidget {
Q_OBJECT

public:
    explicit HistoryView(int userId, QWidget *parent = nullptr);
    void populateData(); // Function to fill the table with data

private:
    QTableWidget *table;
    int userId;

    void setupUI();
};

#endif // HISTORYVIEW_H
