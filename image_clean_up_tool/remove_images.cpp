#include "remove_images.hpp"
#include "ui_remove_images.h"

#include "utils/iterator_create.hpp"

#include <vp_tree.hpp>
#include <annoy/src/kissrandom.h>
#include <annoy/src/annoylib.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/img_hash.hpp>
#include <opencv2/core/ocl.hpp>

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <execution>

using namespace std;

namespace{

struct dist_compare
{
    using value_type = std::tuple<QString, cv::Mat, int>;
    using algo_type = cv::Ptr<cv::img_hash::ImgHashBase>;

    explicit dist_compare(algo_type algo) :
        algo_(algo){}

    double operator()(value_type const &lhs, value_type const &rhs) const
    {
        return algo_->compare(std::get<1>(lhs), std::get<1>(rhs));
    }

private:
    algo_type algo_;
};

size_t remove_duplicate_seq(cv::img_hash::ImgHashBase *hash,
                            std::vector<dist_compare::value_type> const &hash_arr,
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
            for(size_t j = i + 1; j < hash_arr.size(); ++j){
                if(QFile(std::get<0>(hash_arr[j])).exists()){
                    auto const cmp_value = hash->compare(std::get<1>(hash_arr[i]), std::get<1>(hash_arr[j]));
                    if(cmp_value < threshold){
                        similar_url.emplace_back(std::get<0>(hash_arr[j]), std::get<2>(hash_arr[j]));
                    }
                }
            }
            if(!similar_url.empty()){
                similar_url.emplace_back(url, std::get<2>(hash_arr[i]));
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

    qDebug()<<__func__<<": remove "<<removed_file<<" files";

    return removed_file;
}

size_t remove_duplicate_par(cv::img_hash::ImgHashBase *hash,
                            std::vector<dist_compare::value_type> const &hash_arr,
                            double threshold)
{
    size_t removed_file = 0;
    std::mutex mutex_rm;
    std::for_each(std::execution::par, std::begin(hash_arr), std::end(hash_arr), [&](auto const &val)
    {
        auto const &url = std::get<0>(val);
        if(QFile(url).exists()){
            std::vector<std::pair<QString, int>> similar_url;
            for(size_t j = 0; j < hash_arr.size(); ++j){
                if(QFile(std::get<0>(hash_arr[j])).exists()){
                    auto const cmp_value = hash->compare(std::get<1>(val), std::get<1>(hash_arr[j]));
                    if(cmp_value < threshold){
                        similar_url.emplace_back(std::get<0>(hash_arr[j]), std::get<2>(hash_arr[j]));
                    }
                }
            }
            if(!similar_url.empty()){
                similar_url.emplace_back(url, std::get<2>(val));
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
    });

    return  removed_file;
}

size_t remove_duplicate(vp_tree<dist_compare::value_type, dist_compare> &tree,
                        std::vector<QString> const &image_urls,
                        double threshold)
{
    std::vector<dist_compare::value_type> closest_hash;
    std::vector<double> distance;
    size_t removed_file = 0;
    for(size_t i = 0; i < image_urls.size(); ++i){
        if(i % 100 == 0){
            qDebug()<<"removed_file:"<<removed_file<<",compare :"<<image_urls[i];
        }
        if(QFile(image_urls[i]).exists()){
            tree.search(tree.get_items()[i], 10, closest_hash, distance,
                        [=](double val){ return val <= threshold; });
            if(i % 100 == 0){
                qDebug()<<"closest_hash:"<<closest_hash.size();
            }
            if(closest_hash.size() > 1){
                std::vector<std::pair<QString, int>> similar_url;
                for(size_t j = 0; j != closest_hash.size(); ++j){
                    similar_url.emplace_back(std::get<0>(tree.get_items()[j]), std::get<2>(tree.get_items()[j]));
                }
                //keep image with biggest size
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

    qDebug()<<__func__<<": remove "<<removed_file<<" files";

    return removed_file;
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

void remove_images::on_pushButtonSelectFolder_clicked()
{
    auto const dir =
            QFileDialog::getExistingDirectory(this, tr("Select folder to move the file"), ui->lineEditFolder->text());
    ui->lineEditFolder->setText(dir);
}

void remove_images::on_pushButtonRemove_clicked()
{
    load_image_urls();
    if(!image_urls_.empty()){
        auto phash = cv::img_hash::PHash::create();
        std::vector<dist_compare::value_type> hash_arr;
        for(size_t i = 0; i != image_urls_.size(); ++i){
            if(i % 100 == 0){
                qDebug()<<"compute hash of:"<<image_urls_[i];
            }
            cv::Mat const img = cv::imread(image_urls_[i].toStdString());
            if(!img.empty()){
                cv::Mat hash;
                phash->compute(img, hash);
                hash_arr.emplace_back(image_urls_[i], hash, img.cols * img.rows);
            }
        }

        size_t remove_size = 0;
        if(ui->checkBoxFast->isChecked()){
            dist_compare dc(std::move(phash));
            vp_tree<dist_compare::value_type, dist_compare> tree(std::move(dc));
            tree.create(std::move(hash_arr));
            remove_size = remove_duplicate(tree, image_urls_, ui->spinBoxThreshold->value());
        }else{
            remove_size = remove_duplicate_seq(phash.get(), hash_arr, ui->spinBoxThreshold->value());
        }
        ui->labelImageSize->setText(tr("Image size = %1").arg(image_urls_.size() - remove_size));
        QMessageBox::information(this, tr("image_clean_up_tool"), tr("Remove %1 file(s)").arg(remove_size));
    }
}

void remove_images::load_image_urls()
{
    if(!QDir(ui->lineEditFolder->text()).exists()){
        QMessageBox::warning(this, tr("image_clean_up_tool"), tr("Image folder do not exist"));
        return;
    }

    auto const iterate_flag = ui->checkBoxRecursive->isChecked() ?
                QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    auto dir_it = create_image_iterator(ui->lineEditFolder->text(), iterate_flag);
    image_urls_.clear();
    while(dir_it.hasNext()){
        image_urls_.emplace_back(QFileInfo(dir_it.next()).absoluteFilePath());
    }
    ui->labelImageSize->setText(tr("Image size = %1").arg(image_urls_.size()));
}
