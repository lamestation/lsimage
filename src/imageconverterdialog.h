#pragma once

#include "imageconverter.h"
#include "ui_imageconverterdialog.h"

#include <QString>
#include <QProcess>

class ImageConverterDialog : public QWidget
{
    Q_OBJECT

    Ui::ImageConverterDialog ui;
    QString _filename;
    ImageConverter _converter;

public:
    explicit ImageConverterDialog(QWidget *parent = 0);
    ~ImageConverterDialog();

public slots:
    void updateImage();
    void newFile();
    void open();
    void openFile(QString name);
    void exportFile();
    void exportFile(QString name);
};
