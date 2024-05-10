#include "../../../include/ui/common/MainWindow.h"
#include "../../../include/ui/front_office/AccountView.h"
#include "../../../include/ui/front_office/HistoryView.h"
#include "../../../include/ui/front_office/WireView.h"
#include "../../../include/ui/back_office/UserManagementView.h"
#include "../../../include/ui/back_office/AccountCreationView.h"
#include "../../../include/ui/front_office/HomeView.h"
#include "../../../include/ui/back_office/AllHistoricView.h"
#include "../../../include/ui/back_office/AccountWireView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QFrame>
#include <QMenuBar>
#include <QStatusBar>
#include <QPropertyAnimation>
#include <iostream>

using namespace std;

MainWindow::MainWindow(const QString& userRole, const int userId, QWidget *parent)
        : QMainWindow(parent), userRole(userRole), userId(userId) {
    setWindowTitle("Retalbank - " + userRole);
    resize(1280, 800);

    // Setup sidebar and content
    setupSidebar();
    setCentralWidget(new QWidget(this));  // Set the main widget

    // Main Layout setup
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget());
    mainLayout->addWidget(navList, 0); // Sidebar takes minimal necessary space
    mainLayout->addWidget(contentStack, 1); // Content takes the rest
    mainLayout->setSpacing(0);  // No space between widgets
    mainLayout->setContentsMargins(0, 0, 0, 0);  // No margin around the widgets

    // Apply stylesheet for QMainWindow
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f0f4f7; /* Main background color */
        }
    )");

    // Update connection for switching views
    connect(navList, &QListWidget::currentRowChanged, this, &MainWindow::switchView);
}

void MainWindow::setupSidebar() {
    // Initialize sidebar
    navList = new QListWidget(this);
    navList->setFocusPolicy(Qt::NoFocus);

    navList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding); // Adjust width and let height expand
    navList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); // Show scrollbar as needed
    navList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Always hide horizontal scrollbar


    contentStack = new QStackedWidget(this);
    if (userRole == "Directeur" || userRole == "Employé" || userRole == "Stage") {
        buttonNames = {"Historique", "Gestion des Utilisateurs", "Virement", "Création de Compte", "Paramètre"};
        iconPaths = {":/history", ":/users-alt", ":/wire", ":/accounts", ":/settings"};

        viewMappings["Historique"] = 0;
        viewMappings["Gestion des Utilisateurs"] = 1;
        viewMappings["Virement"] = 2;
        viewMappings["Création de Compte"] = 3;

        AllHistoricView* allHistoricView = new AllHistoricView(userId);
        AccountCreationView* accountCreationView = new AccountCreationView();
        UserManagementView* userManagementView = new UserManagementView(allHistoricView);

        contentStack->addWidget(allHistoricView);
        contentStack->addWidget(userManagementView);
        contentStack->addWidget(new AccountWireView(userId));
        contentStack->addWidget(accountCreationView);

        connect(accountCreationView, &AccountCreationView::userCreation, userManagementView, &UserManagementView::refreshUserList);
    } else {
        buttonNames = {"Page d'Accueil", "Virements", "Historique", "Paramètre"};
        iconPaths = {":/home", ":/wire", ":/history", ":/settings"};

        viewMappings["Page d'Accueil"] = 0; // HomeView index
        viewMappings["Historique"] = 1; // HistoryView index
        viewMappings["Virements"] = 2;

        contentStack->addWidget(new HomeView(userId));
        contentStack->addWidget(new HistoryView(userId));
        contentStack->addWidget(new WireView(userId));
    }

    for (int i = 0; i < buttonNames.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(QIcon(iconPaths[i]), buttonNames[i]);
        navList->addItem(item);
    }

    // Disconnect button
    QListWidgetItem *disconnectItem = new QListWidgetItem(QIcon(":/disconnect"), "Déconnexion");
    navList->addItem(disconnectItem);

    // Set stylesheet for QListWidget (navList)
    navList->setStyleSheet(R"(
    QListWidget {
        background-color: #ffffff; /* White background for sidebar */
        border-right: 1px solid #eaeaea; /* Light border for a subtle division */
        padding-top: 20px; /* Space on top */
    }

    QListWidget::item {
        color: #627480; /* Soft color for text */
        padding: 15px 20px;
        border-radius: 8px; /* Rounded corners for the list items */
        margin: 4px 15px; /* Space between items and a little horizontal padding */
        font-size: 16px; /* Slightly larger font for readability */
        background-color: #ffffff; /* White background for items */
        border: none; /* Remove any borders */
    }

    QListWidget::item:selected, QListWidget::item:hover {
        color: #2c3e50; /* Dark color for selected/hovered item text */
        background-color: #e8e8e8; /* Slightly different gray for hover and selection */
        border: none; /* Ensure no border on selection */
    }

    QListWidget::item:focus {
        outline: none; /* Removes focus outline */
        border: none; /* Ensure no border when item is focused */
    })");

    // Setup signal for sidebar item selection change to trigger animation
    connect(navList, &QListWidget::itemClicked, this, &MainWindow::updateView);
}



void MainWindow::switchView(int index) {
    // Check if the item has a view associated with it
    QVariant data = navList->item(index)->data(Qt::UserRole);

    if (data.isValid()) {
        int viewIndex = data.toInt();
        contentStack->setCurrentIndex(viewIndex);
    }
}

void MainWindow::updateView(QListWidgetItem *item) {
    QString buttonName = item->text();
    if (buttonName == "Déconnexion") {
        // Handle logout logic
        emit logoutRequested();
    } else if (viewMappings.contains(buttonName)) {
        int index = viewMappings[buttonName];
        contentStack->setCurrentIndex(index);
    }
}