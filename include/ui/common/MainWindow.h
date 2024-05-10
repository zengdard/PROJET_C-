#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(const QString& userRole, int userId, QWidget *parent = nullptr);

private slots:
    void updateView(QListWidgetItem *item);

signals:
    void logoutRequested();

private:
    QString userRole;
    int userId;
    QListWidget *navList{};          // Liste pour la navigation
    QStackedWidget *contentStack{};  // Empileur de widgets pour le contenu
    QMap<QString, int> viewMappings;

    QStringList buttonNames;
    QStringList iconPaths;

    void setupSidebar();

    void switchView(int index);

};

#endif // MAINWINDOW_H
