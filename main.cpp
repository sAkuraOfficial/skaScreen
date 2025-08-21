#include "src/skaScreen.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    skaScreen w;
    w.show();
    return a.exec();
}
