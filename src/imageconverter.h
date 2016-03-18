#pragma once

#include <QString>
#include <QImage>
#include <QColor>

#include "color.h"

class ImageConverter
{

private:
    QString _colorkey;
    ColorTable _colors; 
    QColor _transparent;
    QImage _image;

    float _scale;

    int _framewidth;
    int _frameheight;

    int framecountx, framecounty;
    int _newframewidth;
    int _newframeheight;
    int newwidth, newheight;

    int low, high;
    int mid, lowbreak, highbreak;

    int ceilingMultiple(int x, int multiple);
    void detectDynamicRange(int range);
    int ** buildDataStructure(QImage image);

    QString assembleSpinHeader();

public:
    ImageConverter(QImage image = QImage());

    void loadImage(QImage image);
    void setTransparentColor(QColor color = QColor(255, 0, 255));
    bool setScaleFactor(float scale = 1.0);
    bool setFrameSize(int w, int h);
    bool setDynamicRange(int range = 55);
    bool setColorTable(QString key = "plain");

    QImage chopIntoFrames();
    QImage applyColorFilter(QImage image);

    QImage scaleImage(QImage image, float scale);

    int frameboost();

    QString toSpin(QString filename);
    void preview();
};
