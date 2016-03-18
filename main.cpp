#include <QApplication>
#include <QString>
#include <QDebug>
#include <QImage>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QFile>

#include "img2dat.h"

#include <stdio.h>

#ifndef VERSION
#define VERSION "0.0.0"
#endif

QCommandLineOption argFrameWidth  (QStringList() << "x" << "width",  QObject::tr("Set sprite frame width."),  "PIXELS");
QCommandLineOption argFrameHeight (QStringList() << "y" << "height", QObject::tr("Set sprite frame height."), "PIXELS");
QCommandLineOption argRange       (QStringList() << "r" << "range",  QObject::tr("Set contrast range of output sprite."), "PERCENT");
QCommandLineOption argWrite       (QStringList() << "w" << "write",  QObject::tr("Write the output to file instead of printing."));


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setOrganizationName("LameStation LLC");
    QApplication::setOrganizationDomain("www.lamestation.com");
    QApplication::setApplicationVersion(VERSION);
    QApplication::setApplicationName(QObject::tr("img2dat"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(
            QObject::tr("\nConvert images into LameGFX sprite objects."));

    parser.addOption(argFrameWidth);
    parser.addOption(argFrameHeight);
    parser.addOption(argRange);
    parser.addOption(argWrite);
    parser.addPositionalArgument("file",  QObject::tr("Image to convert"), "FILE");

    parser.process(app);


    // load image
    if (parser.positionalArguments().length() == 0)
    {
        qDebug() << "Error: Must provide filename of image to convert (e.g. 'img2dat awesome.png')";
        return 1;
    }

    QString filename = parser.positionalArguments()[0];

    QImage image;
    QFileInfo fi(filename);

    if (!fi.exists())
    {
        qDebug() << "Error: File not found:" << filename;
        return 1;
    }

    if (!image.load(filename))
    {
        qDebug() << "Error: Failed to load" << filename;
        return 1;
    }


    // set frame size
    int framewidth, frameheight;
    bool ok = 1;

    if (parser.isSet(argFrameWidth))
        framewidth = parser.value(argFrameWidth).toInt(&ok);
    else
        framewidth = image.width();

    if (parser.isSet(argFrameHeight))
        frameheight = parser.value(argFrameHeight).toInt(&ok);
    else
        frameheight = image.height();

    framewidth = qMin(framewidth, image.width());
    frameheight = qMin(frameheight, image.height());

    // set range size
    int range = 55;

    if (parser.isSet(argRange))
    {
        range = parser.value(argRange).toInt(&ok);
        if (!ok)
        {
            qDebug() << "Error: Failed to set parameter";
            return 1;
        }

        range = qMax(range, 0);
        range = qMin(range, 100);
    }

    QFileInfo outfi(filename);
    QString outfilename = "gfx_"+outfi.completeBaseName()+".spin";

    ImageConverter imageConverter(image, outfilename,
                framewidth, frameheight, range);

    imageConverter.preview();
    QString output = imageConverter.exportSpin();

    if (parser.isSet(argWrite))
    {
        QFile file(outfilename);
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream outstream(&file);
            outstream << output;
        }
        file.close();
    }
    else
    {
        printf("%s",qPrintable(output));
    }

    return 0;
}
