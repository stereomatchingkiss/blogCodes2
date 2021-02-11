#ifndef CLUSTER_SIMILAR_IMAGES_HPP
#define CLUSTER_SIMILAR_IMAGES_HPP

#include <QWidget>

#include <opencv2/core.hpp>

#include <memory>
#include <vector>

namespace Ui {
class cluster_similar_images;
}

class cluster_image_viewer;

class cluster_similar_images : public QWidget
{
    Q_OBJECT

public:
    explicit cluster_similar_images(QWidget *parent = nullptr);
    ~cluster_similar_images();

private slots:
    void on_pushButtonCluster_clicked();

    void on_pushButtonShowResults_clicked();

    void on_pushButtonComputeHash_clicked();

private:
    Ui::cluster_similar_images *ui;

    std::vector<std::vector<QString>> clustered_url_;
    std::vector<cv::Mat> hashes_;
    std::unique_ptr<cluster_image_viewer> image_viewer_;
    std::vector<QString> imgs_url_;
    std::vector<unsigned long> labels_;
};

#endif // CLUSTER_SIMILAR_IMAGES_HPP
