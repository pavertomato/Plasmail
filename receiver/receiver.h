/*****************************************
Файл receiver.h
Класс для получения сообщений по imap /
Class for getting information on imap
*******************************************/
#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QVariantList> //список сообщенийк
#include <string>

class QTcpSocket;
class Info; //о отправителе
class QDataStream;

class Receiver : public QObject{

    Q_OBJECT

    static const int timeout = 15000;
    std::string mail_, //почта
    name_, //имя
    server_, //название сервера
    username_, //имя пользователя на этом сервере
    password_; //пароль
    //поток вывода разных сообщений / message stream
    std::ostringstream* log_;
    bool logout_; //нужно ли выводить сообщения? / do we need a log?
    int sock_; //сокет / socket
    Info *info_;
    QTcpSocket *socket_;
    QDataStream *stream_;
    int ready_;

public:
    //Исключения / exceptions
    class WrongPassword //неправильный пароль
    {public: std::string data; WrongPassword(std::string a) : data(a){}};
    class BadServer //неправильный сервер
    {public: std::string data; BadServer(std::string a) : data(a){}};
    class Unconnected //не соединён
    {};

    void connect(Info*); //соединиться с сервером / connect to server
    //вернуть список сообщений в qml / return a list of messages
    Q_INVOKABLE QVariantList messages(); //to qml file
    Q_INVOKABLE QVariantMap settings(); //to qml file
    Q_INVOKABLE void setSettingsData(QVariantMap); //to qml file
    Q_INVOKABLE void getSettingsFromListView();
    Q_INVOKABLE void setSettingsToListView();
    //---
    void emitReceive();
signals:
    //---
    void receive();
    void getSettings();
    void setSettings();
private:
    //отправка сообщения на сервер
    void send_socket(std::string s);
    //прочитать сообщение из сокета
    void read_socket();
    //---
    void readIsDeleted(bool&);
    //прочитать из сокета, проверяя пароль
    void read_socket_with_pass_check();
    //---
    std::string readSocketAnswer();
    std::string readBigSocketAnswer();
    void read_flag(bool*);
    //конец соединения
    void end();
    //количество сообщений в ящике
    int countMessages(std::string);
    //прочитать заголовочную информацию
    std::string readHeader();
    //прочитать сообщение
    std::string readContnt();
    //достать именно заголовок
    std::string fetchSubj(std::string&);
};

#endif
