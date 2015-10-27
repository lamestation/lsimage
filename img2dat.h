#include <QString>
#include <QImage>
#include <QColor>

class Img2Dat {

private:
    QColor transparent;
    QImage image;
    QString filename;
    int framewidth, frameheight;

    int framecountx, framecounty;
    int newframewidth, newframeheight;
    int newwidth, newheight;

    int low, high;
    int mid, lowbreak, highbreak;

    int ceilingMultiple(int x, int multiple);

public:
    Img2Dat(QImage image,
            QString filename,
            int framewidth = 0,
            int frameheight = 0,
            int range = 55);

    void setTransparentColor(QColor color);
    QImage chopIntoFrames();
    void detectDynamicRange(int range = 55);
    int ** buildDataStructure(QImage image);

    QString exportSpin();
    void preview();
};
