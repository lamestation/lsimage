#pragma once

#include <QString>
#include <QColor>
#include <QDebug>

#include "color.h"
#include "lameimage.h"

class ImageConverter
{

private:
    QString _colorkey;
    ColorTable _colors; 
    QColor _transparent;
    LameImage _image;

    float _scale;

    int framecountx, framecounty;
    int _newframewidth;
    int _newframeheight;
    int newwidth, newheight;

    int low, high;
    int mid, lowbreak, highbreak;

    int ceilingMultiple(int x, int multiple);
    void detectDynamicRange(int range);

    QString assembleSpinHeader();

public:
    ImageConverter(LameImage image = LameImage());

    void loadImage(LameImage image);
    void setTransparentColor(QColor color = QColor(255, 0, 255));
    bool setScaleFactor(float scale = 1.0);
    bool setFrameSize(int w, int h);
    bool setFrameWidth(int w);
    bool setFrameHeight(int h);
    bool setDynamicRange(int range = 55);
    bool setColorTable(QString key = "Plain");

    LameImage applyColorFilter(LameImage image);
    QString toSpin(QString filename);

    void preview();
    void recolor();

    LameImage originalImage();
    LameImage resultImage();
};
