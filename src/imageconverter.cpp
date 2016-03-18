#include "imageconverter.h"

#include <QPainter>
#include <QTextStream>
#include <QCoreApplication>
#include <QLabel>
#include <QDebug>

ImageConverter::ImageConverter(QImage image)
{
    // set defaults
    setColorTable();
    setTransparentColor();
    setScaleFactor();
    loadImage(image);
}

void ImageConverter::loadImage(QImage image)
{
    _image = image;
    setFrameSize(_image.width(), _image.height());
    setDynamicRange();
    _image = applyColorFilter(_image);
}

bool ImageConverter::setColorTable(QString key)
{
    if (!_colors.keys().contains(key))
        return false;

    _colorkey = key;
    _image = applyColorFilter(_image);
    return true;
}

bool ImageConverter::setScaleFactor(float scale)
{
    if (scale < 0.0)
        return false;

    _scale = scale;
    return true;
}

bool ImageConverter::setFrameSize(int w, int h)
{
    if (w < 0 || h < 0)
        return false;

    _framewidth = w;
    _frameheight = h;
    return true;
}

bool ImageConverter::setDynamicRange(int range)
{
    if (range < 0 || range > 100)
        return false;

    detectDynamicRange(range);
    return true;
}

void ImageConverter::setTransparentColor(QColor color)
{
    _transparent = color;
}

QImage ImageConverter::chopIntoFrames()
{
    framecountx = _image.width() / _framewidth;
    if (_image.width() % _framewidth > 0)
        framecountx++;
    framecounty = _image.height() / _frameheight;
    if (_image.height() % _frameheight > 0)
        framecounty++;

    _newframewidth  = ceilingMultiple(_framewidth, 8);
    _newframeheight = _frameheight;
    newwidth  = ceilingMultiple(_image.width()  * _newframewidth  / _framewidth,8);
    newheight = ceilingMultiple(_image.height() * _newframeheight / _frameheight, _newframeheight);

    QImage newimage(newwidth, newheight, QImage::Format_RGB32);
    newimage.fill(2);

    QPainter paint(&newimage);

    for (int fy = 0; fy < framecounty; fy++)
    {
        for (int fx = 0; fx < framecountx; fx++)
        {
            paint.drawImage(
                        fx*_newframewidth,   fy*_newframeheight,
                        _image,
                        fx*_framewidth,      fy*_frameheight,
                        _framewidth,         _frameheight
                    );
        }
    }

    paint.end();

    return scaleImage(newimage, _scale);
}


void ImageConverter::detectDynamicRange(int range)
{
    low = high = QColor(_image.pixel(0, 0)).lightness();

    for (int y = 0; y < _image.height(); y++)
    {
        for (int x = 0; x < _image.width(); x++)
        {
            QColor color(_image.pixel(x, y));
            if (color != _transparent)
            {
                int lightness = color.lightness();
                low = qMin(low, lightness);
                high = qMax(high, lightness);
            }
        }
    }

    mid = (low + high) / 2;
    lowbreak = mid - (mid - low) * range / 100;
    highbreak = mid + (high - mid) * range / 100;
}


QImage ImageConverter::applyColorFilter(QImage image)
{
    QImage newimage(image.size(), QImage::Format_Indexed8);
    newimage.setColorTable(_colors[_colorkey]);

    for (int y = 0; y < image.height(); y++)
    {
        for (int x = 0; x < image.width(); x++)
        {
            QColor color = image.pixel(x, y);
            int lightness = color.lightness();

            if (color == _transparent)
            {
                newimage.setPixel(x, y, 2);
            }
            else
            {
                if (lightness >= highbreak)
                    newimage.setPixel(x, y, 1);
                else if (lightness <= lowbreak)
                    newimage.setPixel(x, y, 0);
                else
                    newimage.setPixel(x, y, 3);
            }
        }
    }

    return newimage;
}


int ** ImageConverter::buildDataStructure(QImage image)
{
    int ** imagedata = new int*[image.height()];

    for (int y = 0; y < image.height(); y++)
    {
        imagedata[y] = new int[image.width()];

        for (int x = 0; x < image.width(); x++)
        {
            QColor color = image.pixel(x, y);
            int lightness = color.lightness();

            if (color == _transparent)
            {
                imagedata[y][x] = 2;
            }
            else
            {
                if (lightness >= highbreak)
                    imagedata[y][x] = 1;
                else if (lightness <= lowbreak)
                    imagedata[y][x] = 0;
                else
                    imagedata[y][x] = 3;
            }
        }
    }
    return imagedata;
}


void ImageConverter::preview()
{

    QLabel l;
    l.setPixmap(QPixmap::fromImage(scaleImage(_image, _scale))); 
    l.show();

    qApp->exec();

    l.setPixmap(QPixmap::fromImage(chopIntoFrames()));
    l.show();

    qApp->exec();

}

QString ImageConverter::toSpin(QString filename)
{
    QImage newimage = chopIntoFrames();

    int ** newimagedata = buildDataStructure(newimage);

    QString output;
    QTextStream stream(&output);

    stream  << "' *********************************************************\n"
            << "' " << filename << "\n"
            << "' generated by img2dat " << qApp->applicationVersion() << "\n"
            << "' *********************************************************\n"
            << "PUB Addr\n"
            << "    return @gfx_data\n\n"
            << "DAT\n\n"
            << "gfx_data\n\n"
            << "word    " << frameboost() << "\n"
            << "word    " << _framewidth << ", " << _frameheight << "\n";


    // print data
    unsigned short word = 0;
    for (int fy = 0; fy < framecounty; fy++)
    {
        for (int fx = 0; fx < framecountx; fx++)
        {
            for (int y = 0; y < _newframeheight; y++)
            {
                stream << "word    ";
                word = 0;
                for (int x = 0; x < _newframewidth; x++)
                {
                    if (x % 8 == 0)
                    {
                        word = 0;
                    }
                    word += (newimagedata[fy*_newframeheight+y][fx*_newframewidth+x] << ((x % 8)*2));

                    if (x % 8 == 7)
                    {
                        stream << QString("$%1").arg(word,4,16,QChar('0'));
                        if (x < _newframewidth-1)
                            stream << ",";
                    }
                }

                stream << " ' ";
                for (int x = 0; x < _newframewidth; x++)
                {
                    QString s;
                    switch (newimagedata[fy*_newframeheight+y][fx*_newframewidth+x])
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

    return output;
}


int ImageConverter::ceilingMultiple(int x, int multiple)
{
    if (x % multiple == 0)
        return x;
    else
        return ((x / multiple) + 1) * multiple;
}

QImage ImageConverter::scaleImage(QImage image, float scale)
{
    return image.scaledToWidth( (int) (((float) image.width())*scale) );
}

int ImageConverter::frameboost()
{
    return _framewidth * _frameheight * 2 / 8;
}

