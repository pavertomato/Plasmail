/*****************************************
Файл mailList.qml
Список почты
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

Rectangle {
    id: list

    width: 240 //стандартный размер экрана мобильника (QVGA)
    height: 320

    ListModel
    {
        //модель списка сообщений (то есть хранит в себе строки)
        id: messageModel
        function addMessages()
        {
            clear(); //убрать старые

            //обозначает название первого столбца
            append({"header":"header","body":"body"});
            var data = receiver.messages();
            for (var i=0; i<data.length; i++)
            {
                append(data[i]);
            }
        }
    }

    Connections {
        target: receiver
        //по получению сигнала от плюсов добавить сообщения в список
        onReceive: messageModel.addMessages()
    }

    Connections
    {
        target: sizer
        onResize:
        {
            width = sizer.width();
            height = sizer.height();
        }
    }

    Component
    {
        id: messageDelegate
        Row {
            Rectangle {
                //большой прямоугольник
                color: ListView.isCurrentItem ? "gray" : "light gray";
                width: list.width
                height: list.height/10

                MouseArea
                {
                    anchors.fill: parent

                    onClicked:
                    {
                        console.debug(body);
                        text.text = body;
                    }
                }

                Text { //и текст
                    id: text
                    text: header
                    anchors.fill: parent
                }
            }
        }
    }

    ListView //собсно виджет / widget
    {
        id: listView
        anchors.fill: parent
        model: messageModel
        delegate: messageDelegate
        focus: true
    }
}


