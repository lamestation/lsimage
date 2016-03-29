#include "imageconverterdialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPainter>

ImageConverterDialog::ImageConverterDialog(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    scaleValidator.setRange(0.001, 4.0);
    scaleValidator.setDecimals(3);
    ui.scale->setValidator(&scaleValidator);

    _title = tr("LSImage");
    setWindowTitle(_title);

    _zoom = 4;

    foreach (QString key, _colors.keys())
    {
        ui.colorPalette->addItem(key);
    }

    ui.colorPalette->setCurrentIndex(ui.colorPalette->findText("White on Blue"));

    connect(ui.openButton,  SIGNAL(clicked()),                  this, SLOT(open()));
    connect(ui.exportButton,SIGNAL(clicked()),                  this, SLOT(exportFile()));
    connect(ui.frameEnable, SIGNAL(toggled(bool)),              this, SLOT(setFrameSizeEnabled(bool)));
    connect(ui.frameWidth,  SIGNAL(valueChanged(int)),          this, SLOT(frameWidthChanged(int)));
    connect(ui.frameHeight, SIGNAL(valueChanged(int)),          this, SLOT(frameHeightChanged(int)));

    connect(ui.scale,       SIGNAL(editingFinished()),          this, SLOT(scaleChanged()));
    connect(ui.range,       SIGNAL(valueChanged(int)),          this, SLOT(rangeChanged()));
    connect(ui.offset,      SIGNAL(valueChanged(int)),          this, SLOT(offsetChanged()));
    connect(ui.zoom,        SIGNAL(currentIndexChanged(int)),   this, SLOT(zoomChanged()));
    connect(ui.colorPalette,SIGNAL(currentIndexChanged(int)),   this, SLOT(colorPaletteChanged()));
    connect(ui.transparent, SIGNAL(colorChanged()),             this, SLOT(transparentChanged()));

    setTransparentColor();
    setColorKey("White on Blue");
    disable();
}

ImageConverterDialog::~ImageConverterDialog()
{
}

void ImageConverterDialog::setColorKey(QString key)
{
    _colorkey = key;

    QPalette p = ui.resultViewport->palette();
    p.setColor(QPalette::Window, _colors[_colorkey][2]);
    ui.resultViewport->setPalette(p);
}

void ImageConverterDialog::setTransparentColor(QColor color)
{
    _transparent = color;

    QPalette p = ui.originalViewport->palette();
    p.setColor(QPalette::Window, _transparent);
    ui.originalViewport->setPalette(p);

    _converter.setTransparentColor(_transparent);
}

void ImageConverterDialog::setScale(float scale)
{
    _converter.setScaleFactor(scale);
}

void ImageConverterDialog::setRange(int range)
{
    _converter.setDynamicRange(range);
}

void ImageConverterDialog::setRangeOffset(int offset)
{
    _converter.setRangeOffset(offset);
}


void ImageConverterDialog::disable()
{
    ui.originalImage->hide();
    ui.resultImage->hide();
    setEnabled(false);
}

void ImageConverterDialog::enable()
{
    ui.originalImage->show();
    ui.resultImage->show();
    setEnabled(true);
    setFrameSizeEnabled(ui.frameEnable->isChecked());
}

void ImageConverterDialog::setEnabled(bool enabled)
{
    ui.exportButton->setEnabled(enabled);
    ui.frameEnable->setEnabled(enabled);
    ui.frameWidth->setEnabled(enabled);
    ui.frameHeight->setEnabled(enabled);
    ui.zoom->setEnabled(enabled);
    ui.scale->setEnabled(enabled);
    ui.range->setEnabled(enabled);
    ui.offset->setEnabled(enabled);
    ui.colorPalette->setEnabled(enabled);
    ui.transparent->setEnabled(enabled);
}

void ImageConverterDialog::setFrameSizeEnabled(bool enabled)
{
    LameImage img = _converter.originalImage();

    ui.frameWidth->setEnabled(enabled);
    ui.frameHeight->setEnabled(enabled);
    ui.frameWidth->setMaximum(img.width());
    ui.frameHeight->setMaximum(img.height());

    if (enabled)
    {
        _converter.setFrameSize(
                ui.frameWidth->value(),
                ui.frameHeight->value()
                );
    }
    else
    {
        _converter.setFrameSize(img.width(), img.height());
    }

    updateImage();
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
    QImage blank(QSize(32,32), QImage::Format_RGB32);
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

    setWindowTitle(tr("%1 - %2").arg(QFileInfo(name).fileName()).arg(_title));

    QImage img(name);

    _converter.loadImage(img);
    _converter.setColorTable("Plain");
    _converter.setScaleFactor(1.0);
    _converter.setDynamicRange();
    _converter.recolor();

    enable();
    setScale(ui.scale->text().toFloat());
    setRange(ui.range->value());
    setRangeOffset(ui.offset->value());
    updateImage();
}

void ImageConverterDialog::updateImage()
{
    LameImage original = _converter.originalImage();
    ui.originalSize->setText(tr("(%1, %2)").arg(original.width()).arg(original.height()));

    ui.originalImage->setPixmap(QPixmap::fromImage(original.scaleByFactor(_zoom)));

    LameImage result = _converter.resultImage();
    ui.resultSize->setText(tr("(%1, %2)").arg(result.width()).arg(result.height()));
    result.setColorTable(_colors[_colorkey]);

    result = result.scaleByFactor(_zoom);
    result = result.convertToFormat(QImage::Format_RGB32);

    QPainter paint(&result);
    paint.setPen(QPen(QColor("#007FFF")));

    for (int fy = 0; fy < result.frameCountY(); fy++)
    {
        paint.drawLine(
                0,              fy*result.frameHeight(),
                result.width(), fy*result.frameHeight()
                );
    }

    for (int fx = 0; fx < result.frameCountX(); fx++)
    {
        paint.drawLine(
                fx*result.frameWidth(), 0,
                fx*result.frameWidth(), result.height()
                );
    }

    paint.end();

    ui.resultImage->setPixmap(QPixmap::fromImage(result));

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


void ImageConverterDialog::frameWidthChanged(int w)
{
    _converter.setFrameWidth(w);
    updateImage();
}

void ImageConverterDialog::frameHeightChanged(int h)
{
    _converter.setFrameHeight(h);
    updateImage();
}

void ImageConverterDialog::scaleChanged()
{
    setScale(ui.scale->text().toFloat());
    updateImage();
}

void ImageConverterDialog::rangeChanged()
{
    setRange(ui.range->value());
    updateImage();
}

void ImageConverterDialog::offsetChanged()
{
    setRangeOffset(ui.offset->value());
    updateImage();
}

void ImageConverterDialog::zoomChanged()
{
    _zoom = ui.zoom->currentText().remove(QChar('x'), Qt::CaseInsensitive).toInt();
    updateImage();
}

void ImageConverterDialog::colorPaletteChanged()
{
    setColorKey(ui.colorPalette->currentText());
    updateImage();
}

void ImageConverterDialog::transparentChanged()
{
    setTransparentColor(ui.transparent->color());
    updateImage();
}
