#include "skaScreen.h"
#include "setting.h"
#include "pages/home/home.h"
skaScreen::skaScreen(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置尺寸为480*320
    setFixedSize(SKA_SCREEN_WIDTH, SKA_SCREEN_HEIGHT);
    this->stacked_widget = new QStackedWidget(this);
    //设置stacked_widget自动占满this
    this->setCentralWidget(this->stacked_widget);

    this->stacked_widget->setGeometry(0, 0, SKA_SCREEN_WIDTH, SKA_SCREEN_HEIGHT);

    //加入页面
    this->stacked_widget->addWidget(new home(this));
}

skaScreen::~skaScreen()
{
}
