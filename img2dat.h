#pragma once

#include <QString>
#include <QImage>
#include <QColor>

class ImageConverter
{

private:
    QColor _transparent;
    QImage _image;
    QString _filename;
    int framewidth, frameheight;

    int framecountx, framecounty;
    int newframewidth, newframeheight;
    int newwidth, newheight;

    int low, high;
    int mid, lowbreak, highbreak;

    int ceilingMultiple(int x, int multiple);
    void detectDynamicRange(int range = 55);
    int ** buildDataStructure(QImage image);

    QString assembleSpinHeader();

public:
    ImageConverter(QImage image,
            QString filename,
            int framewidth = 0,
            int frameheight = 0,
            int range = 55);

    void setTransparentColor(QColor color);
    QImage chopIntoFrames();
    int frameboost();

    QString exportSpin();
    void preview();
};
