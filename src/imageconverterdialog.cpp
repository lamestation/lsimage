#include "imageconverterdialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>

ImageConverterDialog::ImageConverterDialog(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.openButton, SIGNAL(clicked()), this, SLOT(open()));
//    connect(ui.exportButton, SIGNAL(triggered()), this, SLOT(save()));

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


void ImageConverterDialog::openFile(QString name)
{
    if (!QFileInfo(name).exists())
    {
        qDebug() << "File does not exist";
        return;
    }

    QImage img(name);

    int framew = 0, frameh = 0;
    QString fw = img.text("LameGFX:framesize:w");
    QString fh = img.text("LameGFX:framesize:h");

    if (!fw.isEmpty())
        framew = fw.toInt();

    if (!fh.isEmpty())
        frameh = fh.toInt();

    qDebug() << framew << frameh;

    ImageConverter imageConverter(img);

    imageConverter.setColorTable("whiteblue");
//    imageConverter.setFrameSize(framew, frameh);
    imageConverter.setScaleFactor(4.0);
    imageConverter.setDynamicRange();
    imageConverter.recolor();

    ui.originalImage->setPixmap(
            QPixmap::fromImage(
                imageConverter.originalImage()
            )
        );

    ui.resultImage->setPixmap(
            QPixmap::fromImage(
                imageConverter.resultImage()
            )
        );
}
