#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "../algo/cvmat_to_pixmap.hpp"
#include "../utility/global.hpp"
#include "setting_window.hpp"

#include <ocv_libs/qt/mat_and_qimage.hpp>

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>

#include <tiny_cnn/tiny_cnn.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    setting_win_(new setting_window(this))
{
    ui->setupUi(this);

    showMaximized();
    maximum_size_ = ui->label_img->size();
    qDebug()<<maximum_size_;
    qDebug()<<size();
    //ui->actionRegion->setIcon(QIcon(":/pics/layer.png"));
    ui->comboBoxTasks->addItems(global::algo_list);
    ui->label_img->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    mat_to_pixmap_ =
            std::make_unique<cvmat_to_pixmap>(maximum_size_);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpenFolder_triggered()
{
    QFileDialog dialog(nullptr, tr("Open File"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setOption(QFileDialog::DontResolveSymlinks);    
    QStringList filters;
    filters << "Image files (*.png *.bmp *.jpg)";
    dialog.setNameFilters(filters);
    if(dialog.exec()){
        file_names_ = dialog.selectedFiles();
        if(!file_names_.isEmpty()){
            ui->label_img->setPixmap(mat_to_pixmap_->to_pixmap(file_names_[0]));
        }
    }
}

void MainWindow::on_actionStart_triggered()
{

}

void MainWindow::on_actionStop_triggered()
{

}

void MainWindow::on_actionSetting_triggered()
{
    setting_win_->exec();
}

void MainWindow::on_actionClearAll_triggered()
{
    file_names_.clear();
}
