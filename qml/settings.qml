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
    //стандартные размеры прямоугольника / size of the rectangle
    property int standartweight : 100
    property int standartheight : 40

    Connections {
        target: receiver
        onSetSettings:
        { //взять параметры из плюсов
            var data = receiver.settings();
            email.text    = data['mail'];
            server.text   = data['server'];
            name.text     = data['name'];
            username.text = data['username'];
            password.text = data['password'];
        }
    }

    Connections
    {
        target: receiver
        onGetSettings:
        { //отдать параметры плюсам
            var data =
            {
                mail     : email.text,
                server   : server.text,
                name     : name.text,
                username : username.text,
                password : password.text
            }
            receiver.setSettingsData(data);
        }
    }

    Connections
    {
        target: sizer
        onResize:
        {
            console.debug("asdf");
            width = sizer.width();
            height = sizer.height();
        }
    }

    Text //подпись к почте
    {
        id : emailtext
        anchors {top:parent.top; left:parent.left}
        text :"e-mail"
    }
    Rectangle //ввод почты
    {
        id:emailrect
        anchors {top:emailtext.bottom; left:emailtext.left}
        width: standartweight
        height:standartheight
        color: "red"
        TextEdit
        {
            id:email
            anchors.fill: parent
        }
    }

    Text //подпись сервера
    {
        id : servertext
        anchors {top:emailrect.bottom; left:parent.left}
        text :"server"
    }
    Rectangle //ввод сервера
    {
        id:serverrect
        anchors {top:servertext.bottom; left:servertext.rigth}
        width: standartweight
        height:standartheight
        color: "red"
        TextEdit
        {
            id:server
            anchors.fill: parent
        }
    }

    Text //имя
    {
        id : nametext
        anchors {top:serverrect.bottom; left:parent.left}
        text :"name"
    }
    Rectangle //ввод имени
    {
        id : namerect
        anchors {top:nametext.bottom; left:nametext.rigth}
        width: standartweight
        height:standartheight
        color: "red"
        TextEdit
        {
            id : name
            anchors.fill : parent
        }
    }

    Text //логин
    {
        id : usernametext
        anchors {top:namerect.bottom; left:parent.left}
        text :"username"
    }
    Rectangle //ввод логина
    {
        id : usernamerect
        anchors {top:usernametext.bottom; left:usernametext.rigth}
        width: standartweight
        height:standartheight
        color: "red"
        TextEdit
        {
            id : username
            anchors.fill : parent
        }
    }

    Text //пароль
    {
        id : passwordtext
        anchors {top:usernamerect.bottom; left:parent.left}
        text :"password"
    }
    Rectangle //ввод пароля
    {
        id : passwordrect
        anchors {top:passwordtext.bottom; left:passwordtext.rigth}
        width: standartweight
        height:standartheight
        color: "red"
        TextEdit
        {
            anchors.fill: parent
            id:password
        }
    }
}
