#pragma once

#include <QLabel>

class ColorChooser : public QLabel
{
    Q_OBJECT

public:
    explicit ColorChooser(QWidget *parent = 0);
    ~ColorChooser();
    
    QColor _color;

public slots:
    QColor color();
    void setColor(QColor color);

signals:
    void colorChanged();

protected:
    void mousePressEvent(QMouseEvent *event);
    
};
