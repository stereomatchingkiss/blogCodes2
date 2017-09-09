import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Page{
    signal linkActivated(var link)

    property alias text : message_text.text

    ColumnLayout{
        anchors.fill: parent

        GroupBox{
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 10
            ColumnLayout{
                anchors.fill: parent
                Flickable{
                    id: flickable
                    clip: true
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    contentWidth: parent.width
                    contentHeight: message_text.height
                    ScrollBar.vertical: ScrollBar{
                        active: true
                    }
                    Text{
                        id: message_text
                        wrapMode: Text.Wrap
                        anchors.left: parent.left
                        anchors.right: parent.right
                        onLinkActivated:{
                            linkActivated(link)
                        }
                    }
                }
            }
        }
    }
}
