#include <QApplication>
#include <QLabel>
#include <QString>
#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QFile>

#include <stdio.h>

#ifndef VERSION
#define VERSION "0.0.0"
#endif

QCommandLineOption argFrameWidth  (QStringList() << "x" << "width",  QObject::tr("Set sprite frame width."),  "PIXELS");
QCommandLineOption argFrameHeight (QStringList() << "y" << "height", QObject::tr("Set sprite frame height."), "PIXELS");
QCommandLineOption argRange       (QStringList() << "r" << "range",  QObject::tr("Set contrast range of output sprite."), "PERCENT");
QCommandLineOption argWrite       (QStringList() << "w" << "write",  QObject::tr("Write the output to file instead of printing."));


int ceilingMultiple(int x, int multiple)
{
    if (x % multiple == 0)
        return x;
    else
        return ((x / multiple) + 1) * multiple;
}

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

    parser.addOption(argFrameWidth);
    parser.addOption(argFrameHeight);
    parser.addOption(argRange);
    parser.addOption(argWrite);
    parser.addPositionalArgument("file",  QObject::tr("Image to convert"), "FILE");

    parser.process(app);


    QColor transparent(255, 0, 255);

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
        range = qMax(range, 0);
        range = qMin(range, 100);
    }

    if (!ok)
    {
        qDebug() << "Error: Failed to set parameter";
        return 1;
    }

        
    // chop into frames
    int framecountx = image.width() / framewidth;
    if (image.width() % framewidth > 0)
        framecountx++;
    int framecounty = image.height() / frameheight;
    if (image.height() % frameheight > 0)
        framecounty++;

    int newframewidth  = ceilingMultiple(framewidth, 8);
    int newframeheight = frameheight;
    int newwidth  = ceilingMultiple(image.width()  * newframewidth  / framewidth,8);
    int newheight = ceilingMultiple(image.height() * newframeheight / frameheight, newframeheight);

//    qDebug() << "   Frame Count: (" << framecountx << "," << framecounty << ")";
//    qDebug() << "Old Frame Size: (" << framewidth << "," << frameheight << ")";
//    qDebug() << "New Frame Size: (" << newframewidth << "," << newframeheight << ")";
//    qDebug() << "New Image Size: (" << newwidth << "," << newheight << ")";

    QImage newimage(newwidth, newheight, QImage::Format_RGB32);
    newimage.fill(transparent);

    QPainter paint(&newimage);

    for (int fy = 0; fy < framecounty; fy++)
    {
        for (int fx = 0; fx < framecountx; fx++)
        {
            paint.drawImage(fx*newframewidth,   fy*newframeheight,
                            image,
                            fx*framewidth,      fy*frameheight,
                            framewidth,         frameheight);
        }
    }

    // detect dynamic range
    int low, high;
    low = high = QColor(image.pixel(0, 0)).lightness();

    for (int y = 0; y < image.height(); y++)
    {
        for (int x = 0; x < image.width(); x++)
        {
            QColor color(image.pixel(x, y));
            if (color != transparent)
            {
                int lightness = color.lightness();
                low = qMin(low, lightness);
                high = qMax(high, lightness);
            }
        }
    }


    // calculate range breakpoints
    int mid = (low + high) / 2;
    int lowbreak = mid - (mid - low) * range / 100;
    int highbreak = mid + (high - mid) * range / 100;

//    qDebug() << " Dynamic Range: ( L" << low << ", M" << mid << ", H" << high << ")";
//    qDebug() << "        Breaks: (" << lowbreak << "," << highbreak << ")" << range << "%";



    // generate data structure
    int ** newimagedata = new int*[newimage.height()];


    for (int y = 0; y < newimage.height(); y++)
    {
        newimagedata[y] = new int[newimage.width()];

        for (int x = 0; x < newimage.width(); x++)
        {
            QColor color = newimage.pixel(x, y);
            int lightness = color.lightness();
            if (color == transparent)
            {
                newimagedata[y][x] = 2;
            }
            else
            {
                if (lightness >= highbreak)
                    newimagedata[y][x] = 1;
                else if (lightness <= lowbreak)
                    newimagedata[y][x] = 0;
                else
                    newimagedata[y][x] = 3;
            }
        }
    }

    QString output;
    QTextStream stream(&output);
    QFileInfo outfi(filename);

    QString outfilename = "gfx_"+outfi.completeBaseName()+".spin";

    stream  << "' *********************************************************\n"
            << "' " << outfilename << "\n"
            << "' generated by img2dat " << app.applicationVersion() << "\n"
            << "' *********************************************************\n"
            << "PUB Addr\n"
            << "    return @data\n\n"
            << "DAT\n\n"
            << "data\n";

    // print data
    unsigned short word = 0;
    for (int fy = 0; fy < framecounty; fy++)
    {
        for (int fx = 0; fx < framecountx; fx++)
        {
            for (int y = 0; y < newframeheight; y++)
            {
                word = 0;
                for (int x = 0; x < newframewidth; x++)
                {
                    if (x % 8 == 0)
                    {
                        word = 0;
                    }
                    word += (newimagedata[fy*newframeheight+y][fx*newframewidth+x] << ((x % 8)*2));

                    if (x % 8 == 7)
                    {
                        stream << QString("$%1").arg(word,4,16,QChar('0'));
                        if (x < newframewidth-1)
                            stream << ",";
                    }

                }

                stream << " ' ";
                for (int x = 0; x < newframewidth; x++)
                {
                    QString s;
                    switch (newimagedata[fy*newframeheight+y][fx*newframewidth+x])
                    {
                        case 2: s = " ";
                                break;
                        case 1: s = "░";
                                break;
                        case 3: s = "▓";
                                break;
                        case 0: s = "█";
                                break;
                    }
                    stream << s;
                }

                stream << "\n";
            }
            stream << "\n";
        }
    }

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
