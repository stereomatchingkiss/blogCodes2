import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

import QtMultimedia 5.9

import CVLogics 1.0

ApplicationWindow {
    id: win
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    Camera{
        id: camera
    }

    ObjDetector{
        id : obj_detector

        height: win.height - 60
        width: win.width
        visible: false

        onMessage: {
            msg_text.text = message
        }

        onObjectDetected: {
            console.log("object detected")
            obj_detector.visible = true
            busy.running = false
            btn.enabled = true
        }
    }

    BusyIndicator{
        id: busy

        anchors.centerIn: parent
    }

    VideoOutput{
        id: video

        height: obj_detector.height
        width: obj_detector.width
        source: camera
    }

    Column{
        anchors.top: obj_detector.bottom
        Text{
            id: msg_text
            width: win.width
        }

        Button{
            id: btn
            width: win.width
            text: obj_detector.visible ?  "Start Camera" : "Analyze"

            onClicked: {
                if(obj_detector.visible){
                    console.log("set video to visible")
                    obj_detector.visible = false
                    video.visible = true
                }else{
                    console.log("start detect")
                    obj_detector.detect(camera)
                    video.visible = false
                    busy.running = true
                    btn.enabled = false
                }
            }
        }
    }
}
