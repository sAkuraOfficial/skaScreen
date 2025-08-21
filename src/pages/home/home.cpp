#include "home.h"
#include "../../utils/blur.h"
#include <qstyleoption.h>
home::home(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    QVector<BlurEffect *> effects;
    for (int i = 0; i <= 4; i++)
    {
        auto *blur = new BlurEffect();
        blur->set_blur_radius(32); // 可调整模糊半径
        blur->set_border_radius(8);
        effects.append(blur);
    }

    // 设置到widget上
    ui.widget->setGraphicsEffect(effects[0]);
    ui.widget_2->setGraphicsEffect(effects[1]);
    ui.widget_3->setGraphicsEffect(effects[2]);
    ui.widget_4->setGraphicsEffect(effects[3]);
    ui.widget_5->setGraphicsEffect(effects[4]);

}

home::~home()
{
}

void home::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

     QPixmap bg(":/images/images/6k.png");
    //QPixmap bg(":/images/images/9565822.png");
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true); // 开启高质量缩放
    painter.drawPixmap(rect(), bg);
}