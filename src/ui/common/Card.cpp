#include "../../../include/ui/common/Card.h"
#include <QHBoxLayout>
#include <QFrame>

Card::Card(QWidget *parent) : QWidget(parent) {
    setupUI();
}


void Card::setupUI() {
    // Frame as the main container
    frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setStyleSheet("background-color: #ffffff; border: 1px solid #eaeaea; border-radius: 10px;");

    // Main layout within the frame
    mainLayout = new QVBoxLayout(frame);
    frame->setLayout(mainLayout);

    // Set the frame as the main widget of the Card
    QVBoxLayout *frameLayout = new QVBoxLayout(this);
    frameLayout->addWidget(frame);
    setLayout(frameLayout);
}

QVBoxLayout* Card::getMainLayout() {
    return mainLayout;
}


void Card::adjustSize(int width, int height) {
    this->setMaximumSize(width, height);
    this->updateGeometry();
}
