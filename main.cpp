#include "receiver/receiver.h"
#include "receiver/info.h"
#include <QApplication>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeContext>
#include <iostream>
#include <QKeyEvent>

class DeclarativeView : public QDeclarativeView
{
public:
    Receiver *receiver;
protected:
    void keyPressEvent(QKeyEvent *e)
    {
        if (e->key()==Qt::Key_6)
            receiver->emitReceive();
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DeclarativeView view;

    Receiver receiver;
    view.receiver = &receiver;
    view.rootContext()->setContextProperty("receiver", &receiver);

#ifdef Q_OS_SYMBIAN
    view.setSource(QUrl::fromLocalFile("e:/qml/main.qml"));
    view.showFullScreen();
#else
    view.setSource(QUrl::fromLocalFile("../Plasmail/qml/main.qml"));
    view.show();
#endif

    Info info;
    /*info.mail = "perdukomzxc8@tambo.ru";
    info.server = "imap.tambo.ru";
    info.name = "Передук Сергеев";
    info.username = "perdukomzxc8@tambo.ru";
    info.password = "PowerData";*/
    info.mail = "z_lezhnin@mail2000.ru";
    info.server = "mail2000.ru:143";
    info.name = "Передук Сергеев";
    info.username = "z_lezhnin";
    info.password = "[St]arSc";
    receiver.connect(&info);
    receiver.emitReceive();
    receiver.setSettingsToListView();

    return app.exec();
}
