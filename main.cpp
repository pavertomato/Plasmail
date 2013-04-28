#include "receiver/receiver.h"
#include "receiver/info.h"
#include <QApplication>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDeclarativeView view;

    Receiver receiver;
    view.rootContext()->setContextProperty("receiver", &receiver);

#ifdef Q_OS_SYMBIAN
    view.setSource(QUrl::fromLocalFile("e:/qml/main.qml"));
    view.showFullScreen();
#else
    view.setSource(QUrl::fromLocalFile("../Plasmail/qml/main.qml"));
    view.show();
#endif

    Info info;
    info.mail = "perdukomzxc8@tambo.ru";
    info.server = "imap.tambo.ru";
    info.name = "Передук Сергеев";
    info.username = "perdukomzxc8@tambo.ru";
    info.password = "PowerData";
    receiver.connect(&info);
    receiver.emitReceive();
    receiver.setSettingsToListView();

    return app.exec();
}
