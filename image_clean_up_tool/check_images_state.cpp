#include "check_images_state.hpp"
#include "ui_check_images_state.h"

#include "utils/iterator_create.hpp"

#include <QDebug>
#include <QFile>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace{

QStringList get_files_path(QString const &folder, int recursive)
{
    auto dir_it = create_image_iterator(folder, recursive);
    QStringList results;
    while(dir_it.hasNext()){
        results.push_back(QDir::toNativeSeparators(QFileInfo(dir_it.next()).absoluteFilePath()));
    }

    return results;
}

}

check_images_state::check_images_state(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::check_images_state)
{
    ui->setupUi(this);
}

check_images_state::~check_images_state()
{
    delete ui;
}

void check_images_state::on_pushButtonOK_clicked()
{
    auto const fpaths = get_files_path(ui->lineEditFolder->text(), ui->checkBoxRecursive->isChecked() ?
                                           QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
    image_meet_states_.clear();
    ui->tableWidget->setRowCount(0);
    for(auto const &path : fpaths){
        if(ui->checkBoxCanLoadByOpenCV->isChecked()){
            auto img = cv::imread(path.toStdString());
            if(img.empty()){
                image_meet_states_.push_back(path);
                qDebug()<<__func__<<": "<<path<<" cannot load";
                auto const row = ui->tableWidget->rowCount();
                ui->tableWidget->insertRow(row);
                ui->tableWidget->setItem(row, 0, new QTableWidgetItem(path));
                ui->tableWidget->setItem(row, 1, new QTableWidgetItem("opencv cannot load the image"));
            }
        }
    }
}

void check_images_state::on_pushButtonRemoveImages_clicked()
{
    for(auto const &val : image_meet_states_){
        QFile::remove(val);
    }
}
