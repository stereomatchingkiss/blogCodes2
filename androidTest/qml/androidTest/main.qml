import QtQuick 2.2
import QtQuick.Controls 1.1

Rectangle {
    width: 100
    height: 62

    color: "red"

    Text{
        id: text
        height: 30
        width: parent.width
        anchors.centerIn: parent
    }

    MouseArea{
        anchors.fill: parent

        onClicked: {            
            console.log("clicked\n")
        }
    }
}
