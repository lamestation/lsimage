#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QFileInfo>

#include "imageconverterdialog.h"

#ifndef VERSION
#define VERSION "0.0.0"
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("LameStation LLC");
    QCoreApplication::setOrganizationDomain("www.lamestation.com");
    QCoreApplication::setApplicationVersion(VERSION);
    QCoreApplication::setApplicationName(QObject::tr("LSImage"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(
            QObject::tr("\nGraphics converter and viewer for LameStation."));

    parser.addPositionalArgument("file",  QObject::tr("Image to convert"), "FILE");
    parser.process(app);

    ImageConverterDialog w;

    if (parser.positionalArguments().length() > 0)
    {
        QString filename = parser.positionalArguments()[0];
        QFileInfo fi(filename);

        if (!fi.exists())
        {
            qDebug() << "Error: File not found:" << filename;
            return 1;
        }

        w.openFile(filename);
    }

    w.show();
    return app.exec();
}
