/*****************************************
Файл settings.qml
параметры получателя
*******************************************/
/*=========================================================================
==  Plasmail is free software: you can redistribute it and/or modify     ==
==  it under the terms of the GNU General Public License as published by ==
==  the Free Software Foundation, either version 3 of the License, or    ==
==  (at your option) any later version.                                  ==
==                                                                       ==
==  Plasmail is distributed in the hope that it will be useful,          ==
==  but WITHOUT ANY WARRANTY; without even the implied warranty of       ==
==  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        ==
==  GNU General Public License for more details.                         ==
==                                                                       ==
==  You should have received a copy of the GNU General Public License    ==
==  along with this program.  If not, see <http://www.gnu.org/licenses/>.==
==                                                                       ==
=========================================================================*/
/*Евгений Лежнин <z_lezhnin@mail2000.ru>,
     Егор Лежнин <pavertomato@gmail.com>, Томск, Сибирь*/
import QtQuick 1.0

Rectangle
{
    id : settings
    property int standartweight : 100
    property int standartheight : 40

    Connections {
        target: receiver
        onSetSettings:
        {
            var data = receiver.settings();
            email.text    = data['mail'];
            server.text   = data['server'];
            name.text     = data['name'];
            username.text = data['username'];
            password.text = data['password'];
        }
    }

    Connections {
        target: receiver
        onGetSettings:
        {
            var data = {
                mail     : email.text,
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
            text: "sfa"
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
