#include <QApplication>

#include <imageconverterdialog.h>

#ifndef VERSION
#define VERSION "0.0.0"
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageConverterDialog w;
    w.show();
    return a.exec();
}
