#ifndef CARD_H
#define CARD_H

#include <QWidget>
#include <QVBoxLayout>
#include <QFrame>

class Card : public QWidget {
Q_OBJECT

public:
    explicit Card(QWidget *parent = nullptr);
    QVBoxLayout* getMainLayout();
    void adjustSize(int width, int height);

private:
    QFrame *frame;
    QVBoxLayout *mainLayout;
    void setupUI();


};

#endif // CARD_H

