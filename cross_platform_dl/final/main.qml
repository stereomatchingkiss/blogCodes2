import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import QtMultimedia 5.9

import CVLogics 1.0

ApplicationWindow {
    id: win
    visible: true
    width: Qt.platform.os == "windows" ? 640 : win.width
    height: Qt.platform.os == "windows" ? 480 : win.height
    title: qsTr("Hello World")

    SwipeView {
        id: swipe_view
        anchors.fill: parent
        currentIndex: tab_bar.currentIndex

        ObjectDetector{

        }

        HelpMenu{

        }

        Privacy{

        }
    }

    footer: TabBar
    {
        id: tab_bar

        padding: 10

        currentIndex: swipe_view.currentIndex

        TabButton
        {
            text: qsTr("Camera")
            height: parent.height
            contentItem: Image
            {
                anchors.fill: parent
                fillMode: Qt.KeepAspectRatio
                source: "qrc:/images/camera.png"
            }
            onClicked: {
                //stack_view.pop()
                //stack_view.push("qrc:/ObjectDetector.qml")
            }
        }

        TabButton
        {
            text: qsTr("Helps")
            height: parent.height
            contentItem: Image
            {
                anchors.fill: parent
                fillMode: Qt.KeepAspectRatio
                source: "qrc:/images/helps.png"
            }
            onClicked: {
                //stack_view.pop()
                //stack_view.push("qrc:/HelpMenu.qml")
            }
        }

        TabButton
        {
            text: qsTr("Privacy")
            height: parent.height
            contentItem: Image
            {
                anchors.fill: parent
                fillMode: Qt.KeepAspectRatio
                source: "qrc:/images/privacy.png"
            }
            onClicked: {
                //stack_view.pop()
                //stack_view.push("qrc:/Privacy.qml")
            }
        }
    }
}
