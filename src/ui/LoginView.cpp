#include "../../include/ui/LoginView.h"
#include "../../include/queries/UserQueries.h"
#include "../../include/db/DatabaseManager.h"
#include "../../include/ui/common/MainWindow.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QStyle>
#include <QValidator>
#include <iostream>

LoginView::LoginView(QWidget *parent) : QWidget(parent) {
    this->setWindowTitle("Connexion - Retalbank");
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(50, 50, 50, 50);

    // Titre
    QLabel *titleLabel = new QLabel("Connexion à Retalbank", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    // Champ d'identifiant
    QLabel *usernameLabel = new QLabel("Identifiant :", this);
    layout->addWidget(usernameLabel);
    usernameLineEdit = new QLineEdit(this);
    usernameLineEdit->setPlaceholderText("Entrez votre identifiant");
    usernameLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(".{5,}"), this));
    layout->addWidget(usernameLineEdit);

    // Champ de mot de passe
    QLabel *passwordLabel = new QLabel("Mot de passe :", this);
    layout->addWidget(passwordLabel);
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setPlaceholderText("Entrez votre mot de passe");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(".{5,}"), this));
    layout->addWidget(passwordLineEdit);

    // Bouton de connexion
    loginButton = new QPushButton("Connexion", this);
    QFont buttonFont = loginButton->font();
    buttonFont.setPointSize(16);
    loginButton->setFont(buttonFont);
    loginButton->setStyleSheet("QPushButton { background-color: #2a9df4; color: white; border-radius: 10px; }"
                               "QPushButton:hover { background-color: #187bcd; }");
    layout->addWidget(loginButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginView::onLoginButtonClicked);

    // Taille fixe de la fenêtre et style général
    setFixedSize(500, 600);
    this->setStyleSheet("QWidget { font-family: 'Segoe UI', Arial; } QLineEdit { padding: 10px; border-radius: 5px; border: 1px solid #ccc; } QLabel { font-size: 16px; }");
}

void LoginView::onLoginButtonClicked() {
    QString username = usernameLineEdit->text();
    QString password = passwordLineEdit->text();

    UserQueries userQueries(DatabaseManager::getInstance()->getConnection());

    if (userQueries.checkLogin(username.toStdString(), password.toStdString())) {
        QString userRole = QString::fromStdString(userQueries.getRole(username.toStdString()));
        const int userId = userQueries.getUserId(username.toStdString());  // Fetch the user ID
        emit loginSuccessful(userRole, userId);  // Pass userRole and userId
    } else {
        QMessageBox::warning(this, "Échec de la connexion", "Identifiant ou mot de passe incorrect.");
    }
}
