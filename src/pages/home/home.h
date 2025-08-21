#pragma once

#include "ui_home.h"
#include <QWidget>
#include <QPainter>

class home : public QWidget
{
    Q_OBJECT

  public:
    home(QWidget *parent = nullptr);
    ~home();

  private:
    Ui::homeClass ui;

  protected:
    void paintEvent(QPaintEvent *event) override;
};
