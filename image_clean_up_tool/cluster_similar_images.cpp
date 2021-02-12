#include "cluster_similar_images.hpp"
#include "ui_cluster_similar_images.h"

#include "cluster_image_viewer.hpp"

#include "utils/iterator_create.hpp"
#include "utils/utility.hpp"

#include <opencv2/img_hash.hpp>

#include <dlib/clustering.h>

#include <QCheckBox>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageReader>
#include <QMessageBox>

cluster_similar_images::cluster_similar_images(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cluster_similar_images),
    image_viewer_(std::make_unique<cluster_image_viewer>())
{
    ui->setupUi(this);

    ui->comboBoxGraphBasedMethod->addItem(tr("Chinese whisper"));
    ui->comboBoxGraphBasedMethod->addItem(tr("Newman"));
}

cluster_similar_images::~cluster_similar_images()
{
    delete ui;
}

void cluster_similar_images::on_pushButtonCluster_clicked()
{
    if(!imgs_url_.empty()){
        auto phash = cv::img_hash::PHash::create();
        std::vector<dlib::sample_pair> edges;
        for(size_t  i = 0; i != imgs_url_.size(); ++i){
            for(size_t j = 0; j != imgs_url_.size(); ++j){
                auto const val = phash->compare(hashes_[i], hashes_[j]);
                edges.emplace_back(i, j, val);
            }
        }
        dlib::remove_duplicate_edges(edges);

        std::vector<unsigned long> labels;
        const auto num_clusters = ui->comboBoxGraphBasedMethod->currentIndex() == 0 ?
                    chinese_whispers(edges, labels) : newman_cluster(edges, labels);
        qDebug()<<__func__<<": num clusters = "<<num_clusters<<", num of labels = "<<labels.size();
        clustered_url_.clear();
        for(size_t cluster_id = 0; cluster_id != num_clusters; ++cluster_id){
            std::vector<QString> urls;
            for(size_t j = 0; j != labels.size(); ++j){
                if(cluster_id == labels[j]){
                    urls.emplace_back(imgs_url_[j]);
                }
            }
            clustered_url_.emplace_back(std::move(urls));
        }

        ui->labelInformation->setText(tr("Compute %1 of hashes, num of clusters is %2").
                                      arg(hashes_.size()).arg(num_clusters));
    }

    QMessageBox::information(this, tr("cluster image"), tr("Done"));
}

void cluster_similar_images::on_pushButtonShowResults_clicked()
{
    if(!clustered_url_.empty()){
        image_viewer_->set_clustered_url(clustered_url_);
        image_viewer_->show();
    }
}

void cluster_similar_images::on_pushButtonComputeHash_clicked()
{
    auto const iterate_flag = ui->checkBoxReadImageRecursive->isChecked() ?
                QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;
    auto dir_it = create_image_iterator(ui->lineEditFolderOfImages->text(), iterate_flag);
    auto phash = cv::img_hash::PHash::create();
    hashes_.clear();
    imgs_url_.clear();
    while(dir_it.hasNext()){
        auto const url = QFileInfo(dir_it.next()).absoluteFilePath();
        imgs_url_.emplace_back(url);
        auto img = read_image_by_fstream(url.toStdWString());
        cv::Mat hash;
        phash->compute(img, hash);
        hashes_.emplace_back(std::move(hash));
    }

    ui->labelInformation->setText(tr("Compute %1 of hashes").arg(hashes_.size()));
    QMessageBox::information(this, tr("cluster image"), tr("Done"));
}
