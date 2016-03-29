#include "colorchooser.h"

#include <QColor>
#include <QMouseEvent>
#include <QColorDialog>

ColorChooser::ColorChooser(QWidget *parent) :
    QLabel(parent)
{
}

ColorChooser::~ColorChooser()
{
}

QColor ColorChooser::color()
{
    return _color;
}

void ColorChooser::setColor(QColor color)
{
    _color = color;

    QPalette p = this->palette();
    p.setColor(QPalette::Window, color);
    this->setPalette(p);
}

void ColorChooser::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QColor color = QColorDialog::getColor(_color, this);
        if(color.isValid())
        {
            setColor(color);
            emit colorChanged();
        }
    }
}
