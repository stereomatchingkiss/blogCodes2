import QtQuick 2.7
import QtQuick.Controls 2.2

import QtMultimedia 5.9

import CVLogics 1.0

Item {
    id: obj

    Camera{
        id: camera
    }

    Dialog {
         id: dialog
         modal: true
         standardButtons: Dialog.Ok
         x: (parent.width - width) / 2
         y: (parent.height - height) / 2

         Label{
             id: label
             font.bold: true
             wrapMode: Text.Wrap
             width: parent.width
         }
     }

    ObjDetector{
        id : obj_detector

        height: obj.height - btn.height - 10
        width: obj.width
        visible: false

        onMessage: {
            label.text = msg
            dialog.open()
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
        autoOrientation: true
    }

    Component.onCompleted: {
        btn.enabled = true
    }

    Column{
        anchors.top: obj_detector.bottom

        Button{
            id: btn
            width: obj.width
            enabled: fasle
            text: obj_detector.visible ?  qsTr("Start Camera") : qsTr("Analyze")

            onClicked: {
                if(obj_detector.visible){
                    console.log("set video to visible")
                    obj_detector.visible = false
                    video.visible = true
                    obj_detector.clear_graph()
                }else{
                    console.log("start detect")
                    obj_detector.detect(camera)
                    video.visible = false
                    busy.running = true
                    btn.enabled = false
                }
            }
        }
    }//*/
}
