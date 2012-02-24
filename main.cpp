/*****************************************
Файл main.cpp
Я думаю здесь описание не п
*******************************************/
/**************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
*****************Проект Plasmail -- получение, отправка почты /

  Plasmail – mobile e-mail client

  Background and Motivation:
  Create multifunctional, crossplatform freely-available e-mail client.

  Project Summary:
  New e-mail client for sending mail through smtp, receive through imap. It
will work on many platforms, but the main goal platform is mobile systems,
which can use Qt. We use QtScript for creating interface, because it is a
new feature, which is very suitable with the mobile platforms. We want to
create a simple graphical user interface, which should work for touchscreen
devices as well, as for devices with keyboards. User can change his
settings with a graphical menu, or he can edit a text xml settings file,
which contains all email sender, or receiver settings(besides password).
  Most hard part of the development is a client-server interaction. An
applications uses Qt toolkit for working with sockets, this should increase
number of available platforms. But it can't decrease time needed for
connection to server and we try to make code for interacting with server
more optimized. Another problem is variety of very different servers. We
want to create application, which should work with majority of them, to do
that, we should add many conditional expressions to our code. That's why we
should pay more attention to client-server interaction side, than to the
gui part. An application will have features for using ssl encryption, it is
another useful feature.

  Project goals and future research directions:
  There are no good free mail clients for symbian, there are many good
clients for desktops or some other mobile platforms. We want to create
cross-platform application, which should work on many platforms (it should
work on platforms, that can use Qt toolkit).
**********************************************************
***************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
***************************************************************************
**********************/
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

#include "receiver/receiver.h"
#include "receiver/info.h"
#include "utils/sizer.h"
#include "ui/window.h"
#include <QApplication>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#include <iostream>
#include <QKeyEvent>
#include <QFileInfo>

//переопределение, с целью перехватывания клавиш
class DeclarativeView : public QDeclarativeView
{
public:
    Receiver *receiver;
protected:
    void keyPressEvent(QKeyEvent *e)
    {
        if (e->key()==Qt::Key_6)
            receiver->emitReceive();
        if (e->key()==Qt::Key_1)
            receiver->emitChangeMode();
        QDeclarativeView::keyPressEvent(e);
    }
};

Receiver receiver;
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DeclarativeView view;

    view.receiver = &receiver;
    Sizer sizer;
    view.rootContext()->setContextProperty("receiver", &receiver);
    view.rootContext()->setContextProperty("sizer", &sizer);

    QObject::connect((QObject*)view.engine(),
                     SIGNAL(quit()), &app, SLOT(quit()));

    QUrl source;

#ifdef Q_OS_SYMBIAN //для симбиана другой каталог
    source = QUrl::fromLocalFile("e:/qml/main.qml");
#else
    //source = QUrl::fromLocalFile("../Plasmail/qml/main.qml");
#endif

    if (source.isValid() && QFileInfo(source.toLocalFile()).exists())
    {
        view.setSource(source);
#ifdef Q_OS_SYMBIAN //для симбиана другой каталог
        view.showFullScreen();
#else
        view.show();
        sizer.full = 0;
#endif
    }
    else
    {
        receiver.bQml = 0;
    }

    Info info;
    info.mail = "perdukomzxc8@tambo.ru";
    info.server = "imap.tambo.ru";
    info.name = "Передук Сергеев";
    info.username = "perdukomzxc8@tambo.ru";
    info.password = "PowerData";

    receiver.connect(&info);
    Window *window;
    if (!receiver.bQml)
        window = new Window(&receiver);
    receiver.emitReceive();
    receiver.setSettingsToListView();
    sizer.emitResize();
    if (!receiver.bQml)

#ifdef Q_OS_SYMBIAN //для симбиана другой каталог
        window->showFullScreen();
#else
        window->show();
#endif

    return app.exec();
}
