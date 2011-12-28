/*****************************************
Файл receiver.h
Класс для получения сообщений по imap /
Class for getting information on imap
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
#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject> //нужно для передачи объекта интерфейсу
#include <QVariantList> //список сообщений
#include <string>

class QTcpSocket; //сокет из Qt для отправления по Tcp протоколу
class Info; //о получателе иныорыч / receiver info
class QDataStream; //доставать данные из сокета / Qt again

//получатель, наследуется для передачи информации графическому интерфейсу /
class Receiver : public QObject //inherites QObject for connection with the
{ //graphical user interface
    Q_OBJECT

    //максимальное время ожидания / maximum wait time
    static const int timeout = 15000;
    static const int MAXNMESSAGES=9;
    std::string mail_, //почта
    name_, //имя
    server_, //название сервера
    username_, //имя пользователя на этом сервере
    password_; //пароль
    //поток вывода разных сообщений / message stream
    std::ostringstream* log_;
    bool logout_; //нужно ли выводить сообщения? / do we need a log?
    int sock_; //сокет / socket
    Info *info_; //информация о получателе
    //текущий сокет связи с сервером почты / current mail socket
    QTcpSocket *socket_;
    //для считывания из сокета / записи данных в сокет
    QDataStream *stream_;
    int ready_; //для проверки соединённости с сервером / are we connected?

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
    //графический файл получает параметры / set to qml
    Q_INVOKABLE QVariantMap settings();
    //получение параметров из графичско / get to qml
    Q_INVOKABLE void setSettingsData(QVariantMap);
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
