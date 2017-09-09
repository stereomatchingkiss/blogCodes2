import QtQuick 2.9

Text
{
    wrapMode: Text.Wrap

    text: "<b>LightDetector</b><br>" +
          "<br>" +
          "<br>" +
          "This application take a picture and detect location of 20 different objects." +
          "<br>" +
          "they are aeroplane, bicycle, bird, boat, bottle, bus, car, cat, chair, cow, diningtable, dog, horse," +
          "<br>" +
          "motorbike, person, pottedplant, sheep, sofa, train, tvmonitor." +
          "<br>" +
          "It is aIt is aimed to help C++/Qt/QML/OpenCV developers by showing an example project how to " +
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
          "You can find the source codes at <a href=\"https://github.com/stereomatchingkiss/blogCodes2/tree/master/cross_platform_dl/final\">github</a>" +
          "<br>" +
          "For all other questions, send me an <a href=\"thamngapwei@gmail.com\">email</a>" +
          "<br>" +
          "All icons used in this application are downloaded from <a href=\"https://openclipart.org/\">openclipart</a>."

    onLinkActivated:
    {
        console.log("link press:" + link)
        Qt.openUrlExternally(link)
    }
}
