#pragma once

#include <QStackedWidget>
#include <QtWidgets/QMainWindow>
class skaScreen : public QMainWindow
{
    Q_OBJECT

  public:
    skaScreen(QWidget *parent = nullptr);
    ~skaScreen();

  private:
    QStackedWidget *stacked_widget;
};
