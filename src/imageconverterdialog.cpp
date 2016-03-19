#include "imageconverterdialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

ImageConverterDialog::ImageConverterDialog(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.openButton,  SIGNAL(clicked()), this, SLOT(open()));
    connect(ui.exportButton,SIGNAL(clicked()), this, SLOT(exportFile()));

    newFile();

}

ImageConverterDialog::~ImageConverterDialog()
{

}

void ImageConverterDialog::open()
{
    QString fn = QFileDialog::getOpenFileName(this,
                tr("Open File"), QDir::currentPath(), "PNG Files (*.png);;All Files (*)");

    if (!fn.isEmpty())
        openFile(fn);
}

void ImageConverterDialog::newFile()
{
    QImage blank(QSize(128,128), QImage::Format_RGB32);
    blank.fill(Qt::white);
    _converter.loadImage(blank);
    updateImage();
}

void ImageConverterDialog::openFile(QString name)
{
    if (!QFileInfo(name).exists())
    {
        qDebug() << "File does not exist";
        return;
    }

    _filename = name;

    QImage img(name);

    int framew = 0, frameh = 0;
    QString fw = img.text("LameGFX:framesize:w");
    QString fh = img.text("LameGFX:framesize:h");

    if (!fw.isEmpty())
        framew = fw.toInt();

    if (!fh.isEmpty())
        frameh = fh.toInt();

    qDebug() << framew << frameh;


    _converter.loadImage(img);
    _converter.setColorTable("whiteblue");
//    _converter.setFrameSize(framew, frameh);
    _converter.setScaleFactor(4.0);
    _converter.setDynamicRange();
    _converter.recolor();

    updateImage();
}

void ImageConverterDialog::updateImage()
{
    ui.originalImage->setPixmap(
            QPixmap::fromImage(
                _converter.originalImage()
            )
        );

    ui.resultImage->setPixmap(
            QPixmap::fromImage(
                _converter.resultImage()
            )
        );
}

void ImageConverterDialog::exportFile()
{
    QFileInfo outfi(_filename);
    QString outfilename = outfi.path() + "/gfx_"+outfi.completeBaseName()+".spin";

    exportFile(outfilename);
}

void ImageConverterDialog::exportFile(QString name)
{
    if (QFileInfo(name).exists())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Overwrite existing file?"), tr("%1 already exists. Do you want to overwrite it?").arg(name),
                QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No)
            return;
    }

    QString output = _converter.toSpin(_filename);

    QFile file(name);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream outstream(&file);
        outstream << output;
    }
    file.close();

    qDebug() << "Saved to" << name;
}
