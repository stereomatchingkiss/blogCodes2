#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "change_wrong_image_suffix.hpp"
#include "check_images_state.hpp"
#include "cluster_similar_images.hpp"
#include "convert_label_img_labels.hpp"
#include "convert_yolo_detect_results.hpp"
#include "generate_image_list.hpp"
#include "image_mover.hpp"
#include "image_cropper.hpp"
#include "labels_check.hpp"
#include "mask_adjustment_widget.hpp"
#include "remove_images.hpp"
#include "show_folders_details.hpp"
#include "split_data.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    str_to_widget_.emplace(tr("crop image"), new image_cropper);
    str_to_widget_.emplace(tr("move image"), new image_mover);
    str_to_widget_.emplace(tr("Show folder details"), new show_folders_details);
    str_to_widget_.emplace(tr("Mask adjustment"), new mask_adjustment_widget);
    str_to_widget_.emplace(tr("Remove images"), new remove_images);
    str_to_widget_.emplace(tr("Split data"), new split_data);
    str_to_widget_.emplace(tr("Convert labels"), new convert_label_img_labels);
    str_to_widget_.emplace(tr("Check labels"), new labels_check);
    str_to_widget_.emplace(tr("Generate image list"), new generate_image_list);
    str_to_widget_.emplace(tr("crop image"), new image_cropper);
    str_to_widget_.emplace(tr("Convert yolo detect results"), new convert_yolo_detect_results);
    str_to_widget_.emplace(tr("Change wrong image suffix"), new change_wrong_image_suffix);
    str_to_widget_.emplace(tr("Check image state"), new check_images_state);
    str_to_widget_.emplace(tr("Cluster similar images"), new cluster_similar_images);

    for(auto const &val : str_to_widget_){
        ui->stackedWidget->addWidget(val.second);
        ui->comboBoxPage->addItem(val.first);
    }

    ui->stackedWidget->setCurrentWidget(str_to_widget_["crop image"]);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_comboBoxPage_activated(int)
{
    if(auto it = str_to_widget_.find(ui->comboBoxPage->currentText()); it != std::end(str_to_widget_)){
        ui->stackedWidget->setCurrentWidget(it->second);
    }
}

