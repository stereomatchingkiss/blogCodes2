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
    title: qsTr("LightDetector")

    SwipeView {
        id: swipe_view
        anchors.fill: parent
        currentIndex: tab_bar.currentIndex

        ObjectDetector{
            id: obj_detector

            confident: settings.confident_value
        }

        HelpMenu{

        }

        Settings{
            id : settings
            onChangeCamera: {
                console.log("Change camera")
                obj_detector.changeCamera(device_id)
            }
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
        }

        TabButton
        {
            text: qsTr("Settings")
            height: parent.height
            enabled: false
            contentItem: Image
            {
                anchors.fill: parent
                fillMode: Qt.KeepAspectRatio
                source: "qrc:/images/settings.png"
                opacity: parent.enabled ? 1.0 : 0.5
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
        }
    }
}
