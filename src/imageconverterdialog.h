#pragma once

#include "imageconverter.h"
#include "ui_imageconverterdialog.h"

#include <QString>
#include <QProcess>

class ImageConverterDialog : public QWidget
{
    Q_OBJECT

private:
    Ui::ImageConverterDialog ui;

public:
    explicit ImageConverterDialog(QWidget *parent = 0);
    ~ImageConverterDialog();

public slots:
    void open();
    void openFile(QString name);
};
