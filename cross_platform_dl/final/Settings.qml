import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import QtMultimedia 5.9

Page
{
    signal changeCamera(var device_id)

    property alias confident_value: confident_slider.value

    ColumnLayout{
        anchors.fill: parent

        GroupBox{
            id: box
            title: qsTr("Settings")
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 10

            ColumnLayout{
                anchors.fill: parent
                RowLayout{
                    Layout.margins: 10
                    Label{
                        id: cam_label
                        text: qsTr("Camera")
                        font.bold: true
                    }

                    ComboBox{
                        id: camera_combo
                        Layout.fillWidth: true
                        model: QtMultimedia.availableCameras
                        textRole: "displayName"
                        delegate: ItemDelegate{
                            text: modelData.displayName
                        }
                        onCurrentIndexChanged:{
                            changeCamera(model[currentIndex].deviceId)
                        }
                    }
                }

                RowLayout{
                    Layout.margins: 10
                    Label{
                        text: qsTr("Confident")
                        font.bold: true
                    }

                    Slider {
                        id: confident_slider
                        from: 0.2
                        to: 1.0
                        Layout.fillWidth: true
                    }
                }

                RowLayout{
                    Layout.margins: 10
                    Label{
                        text: qsTr("Confident value = ") + confident_slider.value
                        font.bold: true
                    }
                }
            }
        }
    }
}
