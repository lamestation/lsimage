#pragma once

#include "imageconverter.h"
#include "ui_imageconverterdialog.h"

#include <QString>
#include <QProcess>
#include <QIntValidator>
#include <QDoubleValidator>

class ImageConverterDialog : public QWidget
{
    Q_OBJECT

    Ui::ImageConverterDialog ui;
    QString _filename;
    ImageConverter _converter;

    QColor _transparent;

    float _scale;
    QIntValidator    rangeValidator;
    QDoubleValidator scaleValidator;

    int _zoom;

public:
    explicit ImageConverterDialog(QWidget *parent = 0);
    ~ImageConverterDialog();
    void setTransparentColor(QColor color = QColor(255, 0, 255));
    void setScale(float scale = 1.0);
    void setRange(int range = 55);

public slots:
    void updateImage();
    void newFile();
    void open();
    void openFile(QString name);
    void exportFile();
    void exportFile(QString name);

    void frameWidthChanged(int w);
    void frameHeightChanged(int h);

    void disable();
    void enable();
    void setEnabled(bool enabled);
    void setFrameSizeEnabled(bool enabled);

    void scaleChanged();
    void rangeChanged();
    void zoomChanged();
};
