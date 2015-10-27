#include <QString>
#include <QImage>
#include <QColor>

class Img2Dat {

    private:
        QColor transparent;
        int ceilingMultiple(int x, int multiple);

    public:
        Img2Dat();
        QString convert(QImage image,
                QString filename,
                int framewidth = 0,
                int frameheight = 0,
                int range = 55);
        void setTransparentColor(QColor color);
};
