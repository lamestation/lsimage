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

    bool _enabled;
    QString _filename;
    QString _directory;
    QStringList _files;
    int _fileindex;

    ImageConverter _converter;

    ColorTable _colors; 
    QString _colorkey;
    QColor _transparent;

    float _scale;
    QIntValidator    rangeValidator;
    QDoubleValidator scaleValidator;

    int _zoom;

    QString _title;

    bool eventFilter(QObject * target, QEvent * event);

public:
    explicit ImageConverterDialog(QWidget *parent = 0);
    ~ImageConverterDialog();

    void setColorKey(QString key = "Plain");
    void setTransparentColor(QColor color = QColor(255, 0, 255));
    void setScale(float scale = 1.0);
    void setRange(int range = 55);
    void setRangeOffset(int offset = 0);

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
    void zoomChanged();
    void rangeChanged();
    void offsetChanged();
    void colorPaletteChanged();
    void transparentChanged();

    void adjustZoom(int adjust);
    void previousImage();
    void nextImage();
};
