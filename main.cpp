#include "src/skaScreen.h"
#include <QtWidgets/QApplication>
#include<QFont>
#include<QFontDatabase>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int fontId = QFontDatabase::addApplicationFont(":/fonts/yahei.ttf");
    if (fontId != -1)
    {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty())
        {
            QFont font(fontFamilies.at(0));
            a.setFont(font);
        }
    }
    skaScreen w;
    w.show();
    return a.exec();
}
