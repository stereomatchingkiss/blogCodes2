#include "change_wrong_image_suffix.hpp"
#include "ui_change_wrong_image_suffix.h"

#include "utils/iterator_create.hpp"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>

namespace{

QString get_image_format(QString const &url)
{
    QImageReader reader(url);
    reader.setAutoDetectImageFormat(true);
    reader.setDecideFormatFromContent(true);

    return reader.format();
}

}

change_wrong_image_suffix::change_wrong_image_suffix(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::change_wrong_image_suffix)
{
    ui->setupUi(this);
}

change_wrong_image_suffix::~change_wrong_image_suffix()
{
    delete ui;
}

void change_wrong_image_suffix::on_pushButtonConvert_clicked()
{
    auto dit = create_image_iterator(ui->lineEditFolder->text(),
                                     ui->checkBoxRecursive->isChecked() ? QDirIterator::Subdirectories :
                                                                          QDirIterator::NoIteratorFlags);
    while(dit.hasNext()){
        auto const url = dit.next();
        auto const format = get_image_format(url);
        QFileInfo finfo(url);
        auto const suffix = finfo.suffix() == "jpg" ? "jpeg" : finfo.suffix();
        if(!format.isEmpty() && suffix != format){
            qDebug()<<__func__<<": change suffix of file = "<<finfo.absoluteFilePath()<<", from "
                   <<suffix<<", to "<<format<<". New name is = "
                  <<finfo.absolutePath() + "/" + finfo.completeBaseName() + "." + format;
            QDir().rename(finfo.absoluteFilePath(),
                          finfo.absolutePath() + "/" + finfo.completeBaseName() + "." + format);
        }
    }
}
