#include <QCoreApplication>
#include <QString>
#include <QDebug>
#include <QImage>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QFile>

#include "imageconverter.h"

#include <stdio.h>

#ifndef VERSION
#define VERSION "0.0.0"
#endif

QCommandLineOption argScale (QStringList() << "s" << "scale",  QObject::tr("Scale image by factor)"),  "FACTOR");
QCommandLineOption argFrame (QStringList() << "f" << "frame",  QObject::tr("Cut into frames (ex. 16x8)"),  "WxH");
QCommandLineOption argRange (QStringList() << "r" << "range",  QObject::tr("Set contrast range of output sprite."), "PERCENT");
QCommandLineOption argPrint (QStringList() << "print",         QObject::tr("Print the output."));
//QCommandLineOption argView  (QStringList() << "view",          QObject::tr("Preview the resulting image."));
QCommandLineOption argColor (QStringList() << "c" << "color",  QObject::tr("Change the preview color scheme. Options are 'plain', 'whiteblue', and 'redblack'."), "COLOR");

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCoreApplication::setOrganizationName("LameStation LLC");
    QCoreApplication::setOrganizationDomain("www.lamestation.com");
    QCoreApplication::setApplicationVersion(VERSION);
    QCoreApplication::setApplicationName(QObject::tr("img2dat"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(
            QObject::tr("\nConvert images into LameGFX sprite objects."));

    parser.addOption(argFrame);
    parser.addOption(argScale);
    parser.addOption(argRange);
    parser.addOption(argPrint);
//    parser.addOption(argView);
    parser.addOption(argColor);
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

    if (parser.isSet(argFrame))
    {
        QStringList pieces = parser.value(argFrame).split('x');
        
        if (pieces.size() != 2)
        {
            qDebug() << "Error: Frame size must be given as 'WxH' (e.g. 16x8)";
            return 1;
        }

        framewidth = pieces[0].toInt(&ok);
        if (!ok || framewidth < 0)
        {
            qDebug() << "Error: Frame width must be integer. Value given:" << pieces[0];
            return 1;
        }

        frameheight = pieces[1].toInt(&ok);
        if (!ok || frameheight < 0)
        {
            qDebug() << "Error: Frame height must be integer. Value given:" << pieces[1];
            return 1;
        }
    }
    else
    {
        framewidth = image.width();
        frameheight = image.height();
    }

    framewidth = qMin(framewidth, image.width());
    frameheight = qMin(frameheight, image.height());


    // set scale factor
    float scale;
    if (parser.isSet(argScale))
    {
        scale = parser.value(argScale).toFloat(&ok);
        if (!ok || scale < 0.0)
        {
            qDebug() << "Error: Scale factor must be positive float. Value given:" << scale;
            return 1;
        }
    }
    else
    {
        scale = 1.0;
    }

    // set range size
    int range = 55;

    if (parser.isSet(argRange))
    {
        range = parser.value(argRange).toInt(&ok);
        if (!ok || range < 0 || range > 100)
        {
            qDebug() << "Error: Range is invalid (expected 0-100)";
            return 1;
        }
    }

    QFileInfo outfi(filename);
    QString outfilename = outfi.path() + "/gfx_"+outfi.completeBaseName()+".spin";

    ImageConverter imageConverter(image);

    if (parser.isSet(argColor))
    {
        if (!imageConverter.setColorTable(parser.value(argColor)))
        {
            qDebug() << "Error: Color key is invalid (expected plain, whiteblue, or redblack)";
            return 1;
        }
    }

    imageConverter.setScaleFactor(scale);
    imageConverter.setDynamicRange(range);
    imageConverter.recolor();
    imageConverter.setFrameSize(framewidth, frameheight);

//    if (parser.isSet(argView))
//        imageConverter.preview();

    QString output = imageConverter.toSpin(outfilename);

    if (!parser.isSet(argPrint))
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
        qDebug() << qPrintable(output);
    }

    return 0;
}
