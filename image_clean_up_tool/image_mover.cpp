#include "image_mover.hpp"
#include "ui_image_mover.h"

#include "utils/iterator_create.hpp"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageReader>
#include <QKeyEvent>
#include <QKeySequence>
#include <QImage>
#include <QMessageBox>
#include <QSettings>
#include <QUuid>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <execution>

namespace{

QString const state_image_folder("state_image_folder");
QString const state_index("state_index");
QString const state_table_folders("state_table_folders");
QString const state_table_keys("state_table_keys");

QString create_unique_name_to_move(QString const &target_dir, QString const &img_url)
{
    auto to = target_dir + "/" + QFileInfo(img_url).fileName();
    if(!QFile::exists(to)){
        return to;
    }

    return target_dir + "/" + QUuid::createUuid().toString() + "." + QFileInfo(img_url).suffix();
}

}

image_mover::image_mover(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::image_mover)
{
    ui->setupUi(this);

    setFocusPolicy(Qt::StrongFocus);

    QSettings settings("image_clean_up_tool", "image_mover");
    if(settings.contains(state_image_folder)){
        ui->lineEditImageFolder->setText(settings.value(state_image_folder).toString());
    }
    if(settings.contains(state_index) && !ui->lineEditImageFolder->text().isEmpty()){
        image_index_ = settings.value(state_index).toUInt();
        load_images(image_index_);
    }
    if(settings.contains(state_table_folders)){
        auto const folders = settings.value(state_table_folders).toList();
        ui->tableWidget->setRowCount(folders.size());
        for(int i = 0; i != folders.size(); ++i){
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(folders[i].toString()));
        }
    }
    if(settings.contains(state_table_keys)){
        auto const vals = settings.value(state_table_keys).toList();
        ui->tableWidget->setRowCount(vals.size());
        for(int i = 0; i != vals.size(); ++i){
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(vals[i].toString()));
        }
    }
    ui->labelImage->setScaledContents(false);

    auto *event_eater = new scroll_area_key_press_eater(this);
    ui->scrollArea->installEventFilter(event_eater);
    connect(event_eater, &scroll_area_key_press_eater::key_press, this, &image_mover::keyPressEvent);
}

image_mover::~image_mover()
{
    QSettings settings("image_clean_up_tool", "image_mover");
    settings.setValue(state_image_folder, ui->lineEditImageFolder->text());
    settings.setValue(state_index, image_index_);
    QStringList table_folders, table_keys;
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        if(ui->tableWidget->item(i, 1)){
            table_folders.push_back(ui->tableWidget->item(i, 1)->text());
        }else{
            table_folders.push_back("");
        }
        if(ui->tableWidget->item(i, 0)){
            table_keys.push_back(ui->tableWidget->item(i, 0)->text());
        }else{
            table_keys.push_back("");
        }
    }
    settings.setValue(state_table_folders, table_folders);
    settings.setValue(state_table_keys, table_keys);

    delete ui;
}

void image_mover::on_pushButtonSelectImageFolder_clicked()
{
    select_folder(ui->lineEditImageFolder);
}

void image_mover::set_number(QSize const &img_size)
{
    ui->labelImageNumber->setText(QString("%1/%2, wxh=%3x%4").arg(image_index_ + 1).arg(images_urls_.size()).
                                  arg(img_size.width()).arg(img_size.height()));
}

void image_mover::on_pushButtonLoadImages_clicked()
{
    load_images(0);
}

void image_mover::show_image()
{
    if((image_index_ + 1) <= images_urls_.size()){
        auto const url = images_urls_[image_index_];
        qDebug()<<"url = "<<url;
        QFile file(url);
        if(file.open(QIODevice::ReadOnly)){
            auto buffer = file.readAll();
            auto cimg = cv::imdecode(cv::Mat(1, buffer.size(), CV_8U, buffer.data()), cv::IMREAD_UNCHANGED);
            if(!cimg.empty()){
                set_number(QSize(cimg.cols, cimg.rows));
                if(cimg.cols > cimg.rows){
                    if(cimg.cols > 640){
                        cv::resize(cimg, cimg, cv::Size(640, static_cast<int>(640.0/cimg.cols * cimg.rows)));
                    }
                }else{
                    if(cimg.rows > 480){
                        cv::resize(cimg, cimg, cv::Size(static_cast<int>(480.0/cimg.rows * cimg.cols), 480));
                    }
                }
                if(cimg.channels() == 4){
                    cv::cvtColor(cimg, cimg, cv::COLOR_BGRA2RGBA);
                }
                if(cimg.channels() == 3){
                    cv::cvtColor(cimg, cimg, cv::COLOR_BGR2RGBA);
                }
                if(cimg.channels() == 1){
                    cv::cvtColor(cimg, cimg, cv::COLOR_GRAY2RGB);
                }
                ui->labelImage->setPixmap(QPixmap::fromImage(QImage(cimg.data, cimg.cols, cimg.rows,
                                                                    static_cast<int>(cimg.step[0]),
                                                             QImage::Format_RGBA8888).copy()));
                ui->labelImageName->setText(url);
            }else{
                qDebug()<<QString("Cannot read image %1").arg(url);
                on_pushButtonNext_clicked();
            }
        }else{
            qDebug()<<QString("Cannot read image %1").arg(url);
            on_pushButtonNext_clicked();
        }
    }
}

void image_mover::on_pushButtonPrev_clicked()
{
    if(image_index_ > 0){
        --image_index_;
        show_image();
        ui->spinBoxIndex->setValue(static_cast<int>(image_index_ + 1));
    }
}

void image_mover::on_pushButtonNext_clicked()
{
    if((image_index_ + 1) < images_urls_.size()){
        ++image_index_;
        show_image();
        ui->spinBoxIndex->setValue(static_cast<int>(image_index_ + 1));
    }
}

void image_mover::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<__func__<<event->key();
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        auto const &text = ui->tableWidget->item(i, 0)->text();
        if(!text.isEmpty()){
            auto const key = QKeySequence(text)[0];
            if(key == event->key()){
                auto const &folder = ui->tableWidget->item(i, 1)->text();
                if(!folder.isEmpty()){
                    move_file(folder);
                }
                break;
            }
        }
    }

    if(event->key() == Qt::Key_Left){
        on_pushButtonPrev_clicked();
    }else if(event->key() == Qt::Key_Right){
        on_pushButtonNext_clicked();
    }else{
        QWidget::keyPressEvent(event);
    }
}

void image_mover::load_images(size_t image_index)
{
    if(!QDir(ui->lineEditImageFolder->text()).exists()){
        QMessageBox::warning(this, tr("image_clean_up_tool"), tr("Image folder do not exist"));
        return;
    }

    auto const iterate_flag = ui->checkBoxRecursive->isChecked() ?
                QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    auto dir_it = create_image_iterator(ui->lineEditImageFolder->text(), iterate_flag);
    steps_record_.clear();
    images_urls_.clear();
    while(dir_it.hasNext()){
        images_urls_.emplace_back(QFileInfo(dir_it.next()).absoluteFilePath());
    }
    qDebug()<<__func__<<": image size = "<<images_urls_.size();
    image_index_ = (image_index + 1) <= images_urls_.size() ? image_index : 0;
    ui->spinBoxIndex->setRange(1, static_cast<int>(images_urls_.size()));
    ui->spinBoxIndex->setValue(0);
    qDebug()<<__func__<<" range of spinbox = "<<ui->spinBoxIndex->minimum()<<", "<<ui->spinBoxIndex->maximum();
    show_image();
}

void image_mover::move_file(const QString &target_dir)
{    
    if(!QDir(target_dir).exists()){
        QDir().mkpath(target_dir);
    }
    auto const from = images_urls_[image_index_];
    auto const to = create_unique_name_to_move(target_dir, images_urls_[image_index_]);
    if(auto const success = QDir().rename(from, to); success){
        steps_record_.emplace_back(from, to);
        images_urls_.erase(images_urls_.begin() + static_cast<int>(image_index_));
        show_image();
    }else{
        qDebug()<<QString("Cannot show image %1").arg(images_urls_[image_index_]);
        on_pushButtonNext_clicked();
    }
}

void image_mover::select_folder(QLineEdit *editor)
{
    auto const dir =
            QFileDialog::getExistingDirectory(this, tr("Select folder to move the file"), editor->text());
    editor->setText(dir);
}

void image_mover::on_spinBoxIndex_valueChanged(int arg1)
{
    image_index_ = static_cast<size_t>(arg1 - 1);
    show_image();
}

void image_mover::on_pushButtonRestore_clicked()
{
    if(!steps_record_.empty()){
        auto const &back = steps_record_.back();
        QDir().rename(std::get<1>(back), std::get<0>(back));
        steps_record_.pop_back();
    }
}

void image_mover::on_pushButtonAddNewKey_clicked()
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
}


void image_mover::on_pushButtonDeleteKey_clicked()
{
    if(ui->tableWidget->rowCount() > 0){
        ui->tableWidget->removeRow(ui->tableWidget->rowCount() - 1);
    }
}

scroll_area_key_press_eater::scroll_area_key_press_eater(QObject *parent) :
    QObject(parent)
{

}

bool scroll_area_key_press_eater::eventFilter(QObject*, QEvent *event)
{
    qDebug()<<"scroll area event filter : "<<event->type();
    if(event->type() == QEvent::KeyPress){
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if(ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Right){
           emit key_press(ke);
           return true;
        }
    }
    return QObject::event(event);
}
