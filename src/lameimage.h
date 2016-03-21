#pragma once

#include <QImage>

class LameImage : public QImage
{
    int _framewidth;
    int _frameheight;
    int _framecountx;
    int _framecounty;

    void initImage()
    {
        _framewidth = width();
        _frameheight = height();
        _framecountx = 1;
        _framecounty = 1;
    }


public:
    LameImage() : QImage()
    {
        initImage();
    }

    LameImage(const QImage & image)
        : QImage(image)
    {
        initImage();
    }

    LameImage(const QSize & size, Format format)
        : QImage(size, format)
    {
        initImage();
    }

    LameImage(int width, int height, QImage::Format format)
        : QImage(width, height, format)
    {
        initImage();
    }

    ~LameImage()
    {
    }

    LameImage& operator=(const LameImage & other)
    {
        QImage::operator=(other);
        _framewidth = other._framewidth;
        _frameheight = other._frameheight;
        _framecountx = other._framecountx;
        _framecounty = other._framecounty;
        return *this;
    }

    int frameboost()
    {
        return frameWidth() * frameHeight() * 2 / 8;
    }

    int frameWidth()  { return _framewidth;  }
    int frameHeight() { return _frameheight; }

    int frameCountX() { return _framecountx; }
    int frameCountY() { return _framecounty; }

    bool setFrameSize(int w, int h)
    {
        if (w < 1 || h < 1)
            return false;
    
        _framewidth = w;
        _frameheight = h;

        _framecountx = width() / frameWidth();
        _framecounty = height() / frameHeight();

        if (width() % frameWidth() > 0)
            _framecountx++;

        if (height() % frameHeight() > 0)
            _framecounty++;

        return true;
    }

    LameImage scaleByFactor(float scale)
    {
        LameImage newimage(scaledToWidth( (int) (((float) width())*scale) ));
        newimage.setFrameSize(
                (int) ((float) frameWidth())*scale,
                (int) ((float) frameHeight())*scale
                );
        return newimage;
    }

    LameImage convertToFormat(Format format, Qt::ImageConversionFlags flags = Qt::AutoColor)
    {
        LameImage newimage = QImage::convertToFormat(format, flags);
        newimage.setFrameSize(frameWidth(), frameHeight());
        return newimage;
    }
};

