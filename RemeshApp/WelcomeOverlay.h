#pragma once

#include <QWidget>
#include "ui_WelcomeOverlay.h"

class WelcomeOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeOverlay(QWidget* parent = nullptr);
    ~WelcomeOverlay() override;

signals:
    void importRequested();
    void newProjectRequested();
    void openRecentRequested();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::Form* ui = nullptr;

    void closeOverlay();
};