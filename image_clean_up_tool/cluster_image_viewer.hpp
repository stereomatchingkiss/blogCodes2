#ifndef CLUSTER_IMAGE_VIEWER_HPP
#define CLUSTER_IMAGE_VIEWER_HPP

#include <QWidget>

#include <vector>

namespace Ui {
class cluster_image_viewer;
}

class cluster_image_viewer : public QWidget
{
    Q_OBJECT

public:
    explicit cluster_image_viewer(QWidget *parent = nullptr);
    ~cluster_image_viewer();

    void set_clustered_url(std::vector<std::vector<QString>> clustered_url);

private slots:
    void on_spinBoxImageIndex_valueChanged(int arg1);

    void on_spinBoxImageCluster_valueChanged(int arg1);

private:
    void update_image();

    Ui::cluster_image_viewer *ui;

    std::vector<std::vector<QString>> clustered_url_;
};

#endif // CLUSTER_IMAGE_VIEWER_HPP
