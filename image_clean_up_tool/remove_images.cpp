#include "remove_images.hpp"
#include "ui_remove_images.h"

#include "utils/iterator_create.hpp"
#include "utils/utility.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/img_hash.hpp>
#include <opencv2/core/ocl.hpp>

#include <QCheckBox>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageReader>
#include <QMessageBox>

#include <execution>

using namespace std;

namespace{

struct dist_compare
{
    using value_type = std::tuple<QString, cv::Mat, int>;
    using algo_type = cv::Ptr<cv::img_hash::ImgHashBase>;
};

size_t remove_duplicate_seq(cv::img_hash::ImgHashBase *hash,
                            std::vector<dist_compare::value_type> const &hash_arr,
                            std::vector<dist_compare::value_type> const &hash_arr_can_remove,
                            double threshold)
{
    size_t removed_file = 0;
    for(size_t i = 0; i < hash_arr.size(); ++i){
        auto const &url = std::get<0>(hash_arr[i]);
        if(i % 100 == 0){
            qDebug()<<i<<":removed_file = "<<removed_file<<", compare :"<<url;
        }
        if(QFile(url).exists()){
            std::vector<std::pair<QString, int>> similar_url;
            for(size_t j = 0; j < hash_arr_can_remove.size(); ++j){
                if(QFile(std::get<0>(hash_arr_can_remove[j])).exists() &&
                        std::get<0>(hash_arr_can_remove[j]) != std::get<0>(hash_arr[i])){
                    auto const cmp_value = hash->compare(std::get<1>(hash_arr[i]), std::get<1>(hash_arr_can_remove[j]));
                    if(cmp_value < threshold){
                        similar_url.emplace_back(std::get<0>(hash_arr_can_remove[j]),
                                                 std::get<2>(hash_arr_can_remove[j]));
                    }
                }
            }
            if(!similar_url.empty()){
                if(similar_url.size() == 1){
                    QFile::remove(std::get<0>(similar_url[0]));
                    ++removed_file;
                }else{
                    auto max_it = std::max_element(std::begin(similar_url), std::end(similar_url),
                                                   [](auto const &lhs, auto const &rhs)
                    {
                        return std::get<1>(lhs) < std::get<1>(rhs);
                    });
                    for(auto it = std::begin(similar_url); it != std::end(similar_url); ++it){
                        if(it != max_it && QFile(std::get<0>(*it)).exists()){
                            QFile::remove(std::get<0>(*it));
                            ++removed_file;
                        }
                    }
                }
            }
        }
    }

    qDebug()<<__func__<<": remove "<<removed_file<<" files";

    return removed_file;
}

size_t remove_duplicate_par(cv::img_hash::ImgHashBase *hash,
                            std::vector<dist_compare::value_type> const &hash_arr,
                            std::vector<dist_compare::value_type> const &hash_arr_can_remove,
                            double threshold)
{
    size_t removed_file = 0;
    std::mutex mutex_rm;
    std::for_each(std::execution::par, std::begin(hash_arr), std::end(hash_arr), [&](auto const &val)
    {
        auto const &url = std::get<0>(val);
        if(QFile(url).exists()){
            std::vector<std::pair<QString, int>> similar_url;
            for(size_t j = 0; j < hash_arr_can_remove.size(); ++j){
                if(QFile(std::get<0>(hash_arr_can_remove[j])).exists() &&
                        std::get<0>(hash_arr_can_remove[j]) != url){
                    auto const cmp_value = hash->compare(std::get<1>(val), std::get<1>(hash_arr_can_remove[j]));
                    if(cmp_value < threshold){
                        similar_url.emplace_back(std::get<0>(hash_arr_can_remove[j]),
                                                 std::get<2>(hash_arr_can_remove[j]));
                    }
                }
            }
            if(!similar_url.empty()){
                if(similar_url.size() == 1){
                    std::lock_guard<std::mutex> lock(mutex_rm);
                    QFile::remove(std::get<0>(similar_url[0]));
                    ++removed_file;
                }else{
                    auto max_it = std::max_element(std::begin(similar_url), std::end(similar_url),
                                                   [](auto const &lhs, auto const &rhs)
                    {
                        return std::get<1>(lhs) < std::get<1>(rhs);
                    });
                    for(auto it = std::begin(similar_url); it != std::end(similar_url); ++it){
                        if(it != max_it){
                            std::lock_guard<std::mutex> lock(mutex_rm);
                            if(QFile(std::get<0>(*it)).exists() && QFile::remove(std::get<0>(*it))){
                                ++removed_file;
                            }
                        }
                    }
                }
            }
        }
    });

    return  removed_file;
}



}

remove_images::remove_images(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::remove_images)
{
    ui->setupUi(this);

    cv::ocl::setUseOpenCL(false);
}

remove_images::~remove_images()
{
    delete ui;
}

size_t remove_images::remove_img_size_less_than()
{
    size_t remove_size = 0;
    if(ui->groupBoxRemoveImageLessThanSize->isChecked()){
        for(size_t i = 0; i != image_urls_.size();){
            if(image_urls_can_remove_[i]){
                bool can_read = false;
                QSize size;
                {
                    QImageReader reader(image_urls_[i]);
                    reader.setDecideFormatFromContent(true);
                    can_read = reader.canRead();
                    size = reader.size();
                }
                if(can_read){
                    if(size.width() < ui->spinBoxImageWidth->value() ||
                            size.height() < ui->spinBoxImageHeight->value()){
                        remove_size += QFile::remove(image_urls_[i]);
                        //qDebug()<<__func__<<": "<<image_urls_[i]<<", size = "<<size<<", can remove = "<<can_remove;
                        image_urls_.erase(image_urls_.begin() + i);
                        image_urls_can_remove_.erase(image_urls_can_remove_.begin() + i);
                    }else{
                        ++i;
                    }
                }else{
                    qDebug()<<__func__<<": cannot read image:"<<image_urls_[i];
                    ++i;
                }
            }else{
                ++i;
            }
        }
    }

    return remove_size;
}

size_t remove_images::remove_duplicate_images()
{
    size_t remove_size = 0;
    if(ui->groupBoxRemoveDuplicateImages->isChecked()){
        auto phash = cv::img_hash::PHash::create();
        std::vector<dist_compare::value_type> hash_arr;
        std::vector<dist_compare::value_type> hash_arr_can_remove;
        for(size_t i = 0; i != image_urls_.size(); ++i){
            if(i % 100 == 0){
                qDebug()<<"compute hash of:"<<image_urls_[i];
            }                        
            if(auto const img = read_image_by_fstream(image_urls_[i].toStdWString()); !img.empty()){
                cv::Mat hash;
                phash->compute(img, hash);
                hash_arr.emplace_back(image_urls_[i], hash, img.cols * img.rows);
                if(image_urls_can_remove_[i]){
                    hash_arr_can_remove.emplace_back(image_urls_[i], hash, img.cols * img.rows);
                }
            }else{
                qDebug()<<__func__<<": cannot read image:"<<image_urls_[i];
            }
        }

        if(ui->checkBoxMultiThread->isChecked()){
            remove_size += remove_duplicate_par(phash.get(), hash_arr,
                                                hash_arr_can_remove, ui->spinBoxThreshold->value());
        }else{
            remove_size += remove_duplicate_seq(phash.get(), hash_arr,
                                                hash_arr_can_remove, ui->spinBoxThreshold->value());
        }
    }

    return remove_size;
}

void remove_images::on_pushButtonRemove_clicked()
{
    load_image_urls();
    if(!image_urls_.empty()){
        size_t remove_size = remove_img_size_less_than();
        remove_size += remove_duplicate_images();

        ui->labelImageSize->setText(tr("Image size = %1").arg(image_urls_.size() - remove_size));

        QMessageBox::information(this, tr("image_clean_up_tool"), tr("Remove %1 file(s)").arg(remove_size));
    }
}

void remove_images::load_image_urls()
{
    image_urls_.clear();
    image_urls_can_remove_.clear();
    auto const iterate_flag = ui->checkBoxRecursive->isChecked() ?
                QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    for(int i = 0; i != ui->tableWidget->rowCount(); ++i){
        auto const folder = ui->tableWidget->item(i, 0)->text();
        auto const should_delete = get_checkbox_state(i, 1);
        auto dir_it = create_image_iterator(folder, iterate_flag);
        while(dir_it.hasNext()){
            auto const url = QFileInfo(dir_it.next()).absoluteFilePath();
            image_urls_.emplace_back(url);
            image_urls_can_remove_.emplace_back(should_delete);
        }
    }

    ui->labelImageSize->setText(tr("Image size = %1").arg(image_urls_.size()));
}

void remove_images::on_pushButtonAddFolder_clicked()
{
    auto const row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    auto *cbox = new QCheckBox;
    cbox->setChecked(true);
    ui->tableWidget->setCellWidget(row, 1, place_to_center_widget(cbox));
}

void remove_images::on_pushButtonFolder_clicked()
{
    if(ui->tableWidget->rowCount() > 0){
        ui->tableWidget->removeRow(ui->tableWidget->rowCount() - 1);
    }
}

bool remove_images::get_checkbox_state(int row, int column) const
{
    auto *widget = ui->tableWidget->cellWidget(row, column);
    auto *layout_widget = widget->layout()->itemAt(0)->widget();
    if(layout_widget){
        auto *cbox = qobject_cast<QCheckBox*>(layout_widget);
        if(cbox){
            return cbox->isChecked();
        }
    }

    return false;
}
