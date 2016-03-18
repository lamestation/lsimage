#include "imageconverterdialog.h"

#include <QDebug>

ImageConverterDialog::ImageConverterDialog(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

//    connect(ui.sng, SIGNAL(clicked()), this, SLOT(sng()));
}

ImageConverterDialog::~ImageConverterDialog()
{

}
