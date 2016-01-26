#include "mainwindow.hpp"
#include "object_classifier.hpp"

#include <ocv_libs/core/resize.hpp>

#include <opencv2/highgui.hpp>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>

void dnn_object_classfier();

int main(int argc, char *argv[])
{    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();        

    return a.exec();
}

void dnn_object_classfier()
{
    object_classifier oc("bvlc_googlenet.prototxt",
                         "bvlc_googlenet.caffemodel",
                         "synset_words.txt",
                         cv::Size{224,224});

    for(auto const &name : QDir("cars").entryList()){
        auto img = cv::imread("cars/" + name.toStdString());
        QElapsedTimer time;
        if(!img.empty()){
            time.restart();
            oc.predict(img);
            qDebug()<<time.elapsed();
            ocv::resize_aspect_ratio(img, img, {480,0});
            cv::imshow("car", img);
            int const key = cv::waitKey();
            if(key == 'q'){
                break;
            }
            cv::destroyAllWindows();
        }else{
            qDebug()<<"cars/" + name<<" cannot open";
        }
    }
}
