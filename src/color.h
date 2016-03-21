#include <QMap>
#include <QVector>
#include <QRgb>
#include <QStringList>

class ColorTable
{
    QMap<QString, QVector<QRgb> > colors;

public:
    ColorTable()
    {
        colors["Plain"]         << 0xFF000000 << 0xFFFFFFFF << 0xFFFF00FF << 0xFF7F7F7F;
        colors["White on Blue"] << 0xFF7140FE << 0xFFCCCCCC << 0xFFFF00FF << 0xFFB17DE1;
        colors["Red on Black"]  << 0xFF6F0000 << 0xFFFF0000 << 0xFF000000 << 0xFFCC0000;
    }

    QStringList keys()
    {
        return QStringList(colors.keys());
    }

    const QVector<QRgb> & operator[](const QString & name)
    {
        return colors[name];
    }

};
