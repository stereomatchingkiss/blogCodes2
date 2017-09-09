import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

MessageBoard{

    text: "<b>LightDetector helps</b><br>" +
          "<br>" +
          "<br>" +
          "This application take a picture and detect location of 20 different objects." +
          "<br>" +
          "They are aeroplane, bicycle, bird, boat, bottle, bus, car, cat, chair, cow, diningtable, dog, horse," +
          "<br>" +
          "motorbike, person, pottedplant, sheep, sofa, train, tvmonitor." +
          "<br>" +
          "It is aimed to help C++/Qt/QML/OpenCV developers by showing an example project how to " +
          "<br>" +
          "<br>" +
          "1 : Access camera frame of qml camera" +
          "<br>" +
          "2 : Bundle big binary files by android assets and how to load them" +
          "<br>" +
          "3 : Reuse deep learning models by opencv dnn module" +
          "<br>" +
          "4 : Use QtConcurrent to handle expensive algorithm, keep the ui alive" +
          "<br>" +
          "5 : Communicate between c++ and qml" +
          "<br>" +
          "6 : Draw predicted results by QQuickPaintedItem" +
          "<br>" +
          "7 : Communicate between cv::Mat and QImage" +
          "<br>" +
          "<br>" +
          "<b>How to use</b><br><br><br>" +
          "Use <a href=\"settings\">Settings</a> to setup the confidence value, the lower the value, the more objects " +
          "<br>" +
          "you can detects, with lower accuracy and vice versa. You can pick your camera from settings too." +
          "<br>" +
          "Open your <a href=\"camera\">Camera</a>, click the analyze button detect objects, click Start camera button" +
          "<br>" +
          "to restart the camera." +
          "<br>" +
          "You can find the source codes at <a href=\"https://github.com/stereomatchingkiss/blogCodes2/tree/master/cross_platform_dl/final\">github</a>" +
          "<br>" +
          "For all other questions, send me an <a href=\"thamngapwei@gmail.com\">email</a>" +
          "<br>" +
          "All icons used in this application are downloaded from <a href=\"https://openclipart.org/\">openclipart</a>."

    onLinkActivated:{
        if(link === "settings"){
            tab_bar.currentIndex = 2
        }else if(link === "camera"){
            tab_bar.currentIndex = 0
        }else{
            Qt.openUrlExternally(link)
        }
    }

}
