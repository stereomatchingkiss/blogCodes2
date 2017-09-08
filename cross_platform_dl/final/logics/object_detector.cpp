#include "object_detector.hpp"

//#include <ocv_libs/qt/mat_and_qimage.hpp>

#include <QCamera>
#include <QCameraImageCapture>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QtConcurrent/QtConcurrentRun>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

object_detector::object_detector(QQuickItem *parent) :
    QQuickPaintedItem(parent),
    cam_capture_(nullptr)
{
    connect(&watcher_, &QFutureWatcher<void>::finished, [this]()
    {
        update();
    });

    init();

    try{
#ifdef Q_OS_WIN32
        detector_ = std::make_unique<ssd_detector>("C:/Users/yyyy/Qt/computer_vision_model/caffe/MobileNetSSD_deploy.caffemodel",
                                                   "C:/Users/yyyy/Qt/computer_vision_model/caffe/MobileNetSSD_deploy.prototxt",
                                                   false);
#endif
#ifdef Q_OS_ANDROID
        qDebug()<<"new detector";
        QString const caffe_model = copy_asset_file("assets:/ssd_caffemodel.mp3");
        QString const caffe_proto = copy_asset_file("assets:/ssd_prototxt.mp3");
        detector_.reset(new ssd_detector(caffe_model.toStdString(),
                                         caffe_proto.toStdString(),
                                         false));
#endif
    }catch(std::exception const &ex){
        qDebug()<<"cannot new detector, exception:"<<ex.what();
    }
}

object_detector::~object_detector()
{

}

void object_detector::detect(QObject *qml_cam)
{
    qDebug()<<"detection start";
    if(qml_cam){
        qDebug()<<"qml camera is valid";
        QCamera *camera = qvariant_cast<QCamera*>(qml_cam->property("mediaObject"));
        if(camera){
            qDebug()<<"cam is valid";
            if(!cam_capture_){
                qDebug()<<"new qcamera";
                cam_capture_ = new QCameraImageCapture(camera, this);
                cam_capture_->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
                qDebug()<<"support capture to buffer:"<<
                          cam_capture_->isCaptureDestinationSupported(QCameraImageCapture::CaptureToBuffer);
                connect(cam_capture_, &QCameraImageCapture::imageCaptured, this, &object_detector::image_capture);
                connect(cam_capture_, &QCameraImageCapture::imageSaved, [](int, QString const &file_name)
                {
                    QFile::remove(file_name);
                });
                cam_capture_->capture();
            }else{
                qDebug()<<"start capture";
                cam_capture_->capture();
            }
        }
    }
}

void object_detector::init()
{
    name_to_color_.emplace(std::make_pair("background", cv::Scalar(0,0,0)));
    name_to_color_.emplace(std::make_pair("aeroplane", cv::Scalar(255,0,0)));
    name_to_color_.emplace(std::make_pair("bicycle", cv::Scalar(0,255,0)));
    name_to_color_.emplace(std::make_pair("bird", cv::Scalar(0,0,255)));
    name_to_color_.emplace(std::make_pair("boat", cv::Scalar(0,128,128)));
    name_to_color_.emplace(std::make_pair("bottle", cv::Scalar(128,128,0)));
    name_to_color_.emplace(std::make_pair("bus", cv::Scalar(128,0,128)));
    name_to_color_.emplace(std::make_pair("car", cv::Scalar(128,0,0)));
    name_to_color_.emplace(std::make_pair("cat", cv::Scalar(0,128,0)));
    name_to_color_.emplace(std::make_pair("chair", cv::Scalar(0,0,128)));
    name_to_color_.emplace(std::make_pair("cow", cv::Scalar(0,128,256)));
    name_to_color_.emplace(std::make_pair("diningtable", cv::Scalar(255,255,0)));
    name_to_color_.emplace(std::make_pair("dog", cv::Scalar(0,255,128)));
    name_to_color_.emplace(std::make_pair("horse", cv::Scalar(255,0,128)));
    name_to_color_.emplace(std::make_pair("motorbike", cv::Scalar(255,128,255)));
    name_to_color_.emplace(std::make_pair("person", cv::Scalar(128,128,255)));
    name_to_color_.emplace(std::make_pair("pottedplant", cv::Scalar(255,128,128)));
    name_to_color_.emplace(std::make_pair("sheep", cv::Scalar(255,0,255)));
    name_to_color_.emplace(std::make_pair("sofa", cv::Scalar(60,60,60)));
    name_to_color_.emplace(std::make_pair("train", cv::Scalar(255,60,60)));
    name_to_color_.emplace(std::make_pair("tvmonitor", cv::Scalar(60,60,255)));
}

void object_detector::paint(QPainter *painter)
{
    if(!buffer_.isNull()){
        qDebug()<<"draw image";
        painter->drawImage(buffer_.rect(), buffer_);
        emit objectDetected();
        buffer_ = QImage();
    }else{
        qDebug()<<"buffer is null";
    }
}

void object_detector::draw_detect_results(const ssd_detector::result_type &results, cv::Mat &inout) const
{
    using namespace cv;
    for(auto const &pair : results.first){
        for(auto const &rect : pair.second){
            qDebug()<<"detect "<<pair.first.c_str()<<",height:"<<rect.height<<",width:"<<rect.width
                   <<".("<<rect.y<<","<<rect.height<<")";
            auto it = name_to_color_.find(QString::fromStdString(pair.first));
            if(it != std::end(name_to_color_)){
                rectangle(inout, rect, it->second);
            }else{
                rectangle(inout, rect, Scalar(0, 255, 0));
            }
            putText(inout, pair.first, Point(rect.x, rect.y - results.second.y - 20),
                    FONT_HERSHEY_SIMPLEX, 2, Scalar(0,0,255));
        }
    }
}

void object_detector::image_capture(int id, QImage const &img)
{
    Q_UNUSED(id)

    future_ = QtConcurrent::run(QThreadPool::globalInstance(), [this, img]()
    {
        if(buffer_.format() != QImage::Format_RGB888){
            buffer_ = img.convertToFormat(QImage::Format_RGB888);
        }else{
            buffer_ = img.copy();
        }
        buffer_ = buffer_.scaled(static_cast<int>(width()), static_cast<int>(height()), Qt::AspectRatioMode::KeepAspectRatio);
        qDebug()<<"scaled size 0:"<<buffer_.size();
        qDebug()<<"to mat";
        cv::Mat mat = cv::Mat(buffer_.height(), buffer_.width(), CV_8UC3, buffer_.bits(), buffer_.bytesPerLine());
        qDebug()<<"detect start";
        auto const results = detector_->detect(mat);
        qDebug()<<"draw detect results";
        draw_detect_results(results, mat);
    });
    watcher_.setFuture(future_);
}
