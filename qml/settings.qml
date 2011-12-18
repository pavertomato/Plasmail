import QtQuick 1.0

Rectangle {
    id : settings
    property int standartweight : 100
    property int standartheight : 40

    Connections {
        target: receiver
        onSetSettings:
        {
            console.debug("func: sdafads");
            var data = receiver.settings();
            console.debug('email: ' + data[0][0]);
            emailtext.email.text       = data['email'];
            servertext.server.text     = data['server'];
            nametext.name.text         = data['name'];
            usernametext.username.text = data['username'];
            passwordtext.password.text = data['password'];
        }
    }

    Connections {
        target: receiver
        onGetSettings:
        {
            var data = {
                email    : email.text,
                server   : server.text,
                name     : name.text,
                username : username.text,
                password : password.text
            }
            receiver.setSettingsData(data);
        }
    }

    Text{
        id : emailtext
        anchors { top: parent.top; left: parent.left }
        text :"e-mail"
    }
    Rectangle
    {
        id:emailrect
        anchors { top: emailtext.bottom; left: emailtext.right}
        width: standartweight
        height:standartheight
        color: "red"
        TextEdit{
            id:email
            anchors.fill: parent
            z : 2
            color: "lightsteelblue"
            focus: true
        }
    }

    Text{
        id : servertext
        anchors { top: emailrect.bottom; left: parent.left }
        text :"server"
    }
    Rectangle
    {
        id:serverrect
        anchors { top: servertext.bottom; left: servertext.rigth}
        width: standartweight
        height:standartheight
        color: "red"
        TextEdit{
            id:server
            anchors.fill: parent
            z : 2
            color: "lightsteelblue"
            focus: true
        }
    }

    Text{
        id : nametext
        anchors { top: serverrect.bottom; left: parent.left }
        text :"name"
    }
    Rectangle
    {
        id : namerect
        anchors { top: nametext.bottom; left: nametext.rigth}
        width: standartweight
        height:standartheight
        color: "red"
        TextEdit{
            id : name
            anchors.fill : parent
            z : 2
            color: "lightsteelblue"
            focus: true
        }
    }

    Text{
        id : usernametext
        anchors { top: namerect.bottom; left: parent.left }
        text :"username"
    }
    Rectangle
    {
        id : usernamerect
        anchors { top: usernametext.bottom; left: usernametext.rigth}
        width: standartweight
        height:standartheight
        color: "red"
        TextEdit{
            id : username
            anchors.fill : parent
            z : 2
            color: "lightsteelblue"
            focus: true
        }
    }

    Text{
        id : passwordtext
        anchors { top: usernamerect.bottom; left: parent.left }
        text :"password"
    }
    Rectangle
    {
        id : passwordrect
        anchors { top: passwordtext.bottom; left: passwordtext.rigth}
        width: standartweight
        height:standartheight
        color: "red"
        TextEdit{
            anchors.fill: parent
            id:password
            z : 2
            color: "lightsteelblue"
            focus: true
        }
    }
}
