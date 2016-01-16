#include "mainwindow.hpp"
#include "object_classifier.hpp"

#include <opencv2/highgui.hpp>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    object_classifier oc("bvlc_googlenet.prototxt",
                         "bvlc_googlenet.caffemodel",
                         "synset_words.txt",
                         cv::Size{224,224});

    auto img = cv::imread("cars/Bugatti-Veyron-1_j_2531076b.jpg");
    oc.predict(img);

    return a.exec();
}
