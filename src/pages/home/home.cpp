#include "home.h"
#include "../../utils/blur.h"
#include <qstyleoption.h>
#include "../../utils/img.h"
home::home(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    // 覆盖一下ui文件设置的样式表。
    setStyleSheet(QString::fromUtf8("#widget,#widget_2,#widget_3,#widget_4,#widget_5{\n"
                                    "	border-radius:8;\n"
                                    //"background-color: rgba(222, 204, 214,160);\n"
                                    "}"
    ));

    QVector<BlurEffect *> effects;
    for (int i = 0; i <= 4; i++)
    {
        auto *blur = new BlurEffect();
        blur->set_blur_radius(16); // 可调整模糊半径
        blur->set_border_radius(8);
        effects.append(blur);
    }

    // 设置到widget上
    ui.widget->setGraphicsEffect(effects[0]);
    ui.widget_2->setGraphicsEffect(effects[1]);
    ui.widget_3->setGraphicsEffect(effects[2]);
    ui.widget_4->setGraphicsEffect(effects[3]);
    ui.widget_5->setGraphicsEffect(effects[4]);

    //setAttribute(Qt::WA_TranslucentBackground, true);
    //ui.widget->setAttribute(Qt::WA_TranslucentBackground, true);
    //ui.widget_2->setAttribute(Qt::WA_TranslucentBackground, true);
    //ui.widget_3->setAttribute(Qt::WA_TranslucentBackground, true);
    //ui.widget_4->setAttribute(Qt::WA_TranslucentBackground, true);
    //ui.widget_5->setAttribute(Qt::WA_TranslucentBackground, true);

    // 使用方法
    QPixmap pix(":/images/test/tx.jpg");
    QPixmap roundPix = getRoundPixmap(pix);
    ui.tx ->setPixmap(roundPix);

    QPixmap src(":/images/test/music.jpg");
    int radius = qMin(src.width(), src.height()) * 0.12; // 12% 圆角
    QPixmap rounded = getRoundPixmap_radius(src, radius);
    ui.music_img->setPixmap(rounded);

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
    // QPixmap bg(":/images/images/9565822.png");
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true); // 开启高质量缩放
    painter.drawPixmap(rect(), bg);
}


