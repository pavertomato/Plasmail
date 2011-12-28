import QtQuick 1.0

Rectangle{
    width: 240
    height: 320
    ListView {
        id: listView

        //the model contains the data
        model: winsModel
        currentIndex: 0
        //a list of visual items already have delegates handling their display
        VisualItemModel {
            id: winsModel

            MailList {
            }
            Settings {
                id:settings
            }
        }
    }

    Rectangle{
        anchors.bottom:parent.bottom
        color:"blue"
        width:100
        height:30

        Text {
            text: "settings"
            anchors.fill: parent
        }
        MouseArea {
            anchors.fill: parent

            onClicked: {
                if (listView.currentIndex==0)
                {
                    listView.currentIndex = 1;
                    receiver.setSettingsToListView();
                }
                else
                {
                    listView.currentIndex = 0;
                    receiver.getSettingsFromListView();
                }
            }
        }
    }
}
