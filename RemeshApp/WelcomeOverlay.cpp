#include "WelcomeOverlay.h"

#include <QPushButton>

WelcomeOverlay::WelcomeOverlay(QWidget* parent)
    : QWidget(parent)
{
    ui = new Ui::Form();
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, false);
    setWindowFlags(Qt::FramelessWindowHint);
    setAutoFillBackground(false);

    // TODO: Create a functions to actually open the recent files instead of just closing the overlay
    connect(ui->pushButton_recentFile1, &QPushButton::clicked, this, &WelcomeOverlay::closeOverlay);
    connect(ui->pushButton_recentFile2, &QPushButton::clicked, this, &WelcomeOverlay::closeOverlay);
    connect(ui->pushButton_recentFile3, &QPushButton::clicked, this, &WelcomeOverlay::closeOverlay);
    connect(ui->pushButton_recentFile4, &QPushButton::clicked, this, &WelcomeOverlay::closeOverlay);
}

// Destructor
WelcomeOverlay::~WelcomeOverlay()
{
    delete ui;
}

// Private slot to close the overlay
void WelcomeOverlay::closeOverlay()
{
    hide();
}

// Override the resize event to ensure the overlay resizes with the parent window
void WelcomeOverlay::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}