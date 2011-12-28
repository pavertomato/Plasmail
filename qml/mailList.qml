import QtQuick 1.0

Rectangle {
    id: list

    width: 240
    height: 320

    ListModel {
        id: messageModel
        ListElement
        {
            header : "header"
            content : "fdsa"
        }
        function addMessages()
        {
            clear();
            var data = receiver.messages();
            for (var i=0; i<data.length; i++)
            {
                console.debug(data[i]['header']);
                append(data[i]);
            }
        }
    }

    Connections {
        target: receiver
        onReceive: messageModel.addMessages()
    }

    Component {
        id: messageDelegate
        Row {
            Rectangle {
                color: "red";
                width: 50
                height: 50

                Text {
                    text: header
                    anchors.fill: parent
                }
            }
            //Text { text: content }
        }
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: messageModel
        delegate: messageDelegate
    }
}


