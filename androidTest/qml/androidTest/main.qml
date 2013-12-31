import QtQuick 2.2
import QtQuick.Controls 1.1

Rectangle {
    width: 100
    height: 62

    MouseArea{
        anchors.fill: parent

        onClicked: {
            console.log(simpleJNI.printHelloWorld())
        }
    }
}
