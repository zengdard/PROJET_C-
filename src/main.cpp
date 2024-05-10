#include <QApplication>
#include "../include/ui/LoginView.h"
#include "../include/ui/common/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow* mainWindow = nullptr;
    LoginView* loginView = new LoginView();

    QObject::connect(loginView, &LoginView::loginSuccessful, [&](const QString& userRole, const int userId) {
        if (mainWindow) {
            mainWindow->deleteLater();  // Use deleteLater to safely delete the window
        }
        mainWindow = new MainWindow(userRole, userId);
        mainWindow->show();
        loginView->hide();

        // Note capturing mainWindow by reference now
        QObject::connect(mainWindow, &MainWindow::logoutRequested, [&]() {
            qDebug() << "Logout requested, closing main window and showing login.";
            mainWindow->deleteLater();  // Ensure we clean up the old instance
            mainWindow = nullptr;
            loginView->show();  // Show the login window again
        });
    });

    loginView->show();
    return app.exec();
}
