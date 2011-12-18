#include "receiver.h"
#include "message.h" //информация о сообщении
#include "info.h" //о клиенте imap
#include <QVariantMap>

//base64_encode()
#include "base64.h"
//utf8 to windows1251
#include "u82w1251.hpp"
#include <sstream> //обработка чисел в строках
//std::cerr
#include <iostream>
//memcpy()
#include <string.h>
//exit()
#include <stdlib.h>
#include <QtNetwork/QTcpSocket>
#include <QDataStream>

//конец строки / end of line
std::string END = "\r\n";

//соединиться с сервером / connect to server
void Receiver::connect(Info* info)
{
    ready_ = 0;
    info_ = info;

    //установливаем log
    log_ = new std::ostringstream();
    logout_ = 1;

    //копируем информацию о нас / it's all about us
    mail_ = info->mail;
    server_ = info->server;
    username_ = info->username;
    password_ = info->password;
    name_ = info->name;

    //установить порт
    int i;
    for (i=server_.length(); i>0; i--)
    {
        if (mail_[i]==':')
        {
            break;
        }
    }
    int port;
    if (i!=0)
    {
        std::istringstream istm(server_.substr(i));
        istm >> port;
    }
    else
    {
        port=143;
    }

    socket_ = new QTcpSocket(this);
    socket_->connectToHost(QString::fromStdString(server_),
                           port);
    stream_ = new QDataStream(socket_);
    stream_->setVersion(QDataStream::Qt_4_0);

    if (!socket_->waitForConnected(timeout))
        return;

    read_socket();
    send_socket(std::string("$ LOGIN ")+username_+" "+password_+END);
    read_socket_with_pass_check();

    //логин
    /*read_socket();
    send_socket(std::string("$ LOGIN ")+username_+" "+password_+END);
    read_socket_with_pass_check(); //проверка*/
    //std::cerr << log_->str() << std::endl;

    /*std::string* s = &username_;
    std::string dest64;
    dest64 = base64_encode(
        reinterpret_cast<const unsigned char*>(s->c_str()),
        s->length());
    send_socket(dest64 + '\n');
    read_socket();

    s = &password_;
    dest64 = base64_encode(
        reinterpret_cast<const unsigned char*>(s->c_str()),
        s->length());
    send_socket(dest64+END);
    read_socket_with_pass_check();*/
}

//вернуть список сообщений в qml / return a list of messages to qml file
Q_INVOKABLE QVariantList Receiver::messages()
{
    std::string mailbox = "INBOX"; //почтовый ящик по умолчанию

    int nMessages = countMessages(mailbox); //количество сообщений

    //выбор ящика / select mailbox
    send_socket(std::string("$ SELECT ") + mailbox +END);
    read_socket();

    std::ostringstream omem;
    omem << "$ SEARCH UNDELETED" << END;
    send_socket(omem.str());
    bool *undeleted = new bool[nMessages];
    memset(undeleted,0,nMessages);
    read_flag(undeleted);

    std::ostringstream omem2;
    omem2 << "$ SEARCH RECENT" << END;
    send_socket(omem2.str());
    bool *recent = new bool[nMessages];
    memset(recent,0,nMessages);
    read_flag(recent);

    QVariantList mess; //список сообщений
    std::cerr << nMessages << std::endl;
    for (int i=nMessages-1; i>=0 && i>=nMessages-21; i--) //получить сообщения / get messages
    {
        std::cerr << i << ' ';
        //if (!undeleted[i]) continue;
        //if (!recent[i]) continue;
        Message *mes = new Message(); //новое сообщение

        //получение заголовка
        std::ostringstream omem;
        omem << "$ FETCH " << i+1
             << " (body[header.fields (from subject date)])" << END;
        send_socket(omem.str());
        std::cerr << i << "dsaf";
        mes->header = readHeader();
        std::cerr << i << ") header " << mes->header << std::endl;

        int pos1 = 0, pos2 = 0;

        while (1)
        {
            pos1 = (int)mes->header.find("=?UTF-8?B?",pos1);
            if (pos1==(int)std::string::npos)
                pos1 = (int)mes->header.find("=?utf-8?B?",0);
            if (pos1==(int)std::string::npos) break;
            pos2 = (int)mes->header.find("?=",pos1);
            std::cerr << i << ") was " << mes->header << std::endl;
            mes->header.replace(pos1,pos2-pos1+2,base64_decode(mes->header.substr(pos1+10,pos2-pos1-10)));
            std::cerr << i << ") bec " << mes->header << std::endl;
            if (pos2==(int)std::string::npos) break;
        }

        //получение тела / get a body
        /*std::ostringstream omem2;
        omem2 << "$ FETCH " << i+1 << " BODY[text]" << END;
        send_socket(omem2.str());
        mes->contnt = readContnt();
        std::cerr << i << ") contnt " << mes->contnt << std::endl;*/

        mes->contnt = "";

        //элемент
        QVariantMap map;
        map.insert("header", QString::fromUtf8(mes->header.c_str()));
        map.insert("content",QString::fromStdString(mes->contnt));
        mess << map;
    }

    delete[] undeleted;
    delete[] recent;
    return mess;
}

Q_INVOKABLE QVariantList Receiver::settings() //to qml file
{
    QVariantList list;
    QVariantMap map;
    map.insert("mail",     QString::fromUtf8(info_->mail.c_str()));
    //std::cerr << "info_->mail.c_str(): " << info_->mail.c_str() << std::endl
      //        << QString::fromUtf8(info_->mail.c_str()).toStdString() << std::endl;
    map.insert("server",   QString::fromUtf8(info_->server.c_str()));
    map.insert("name",     QString::fromUtf8(info_->name.c_str()));
    map.insert("username", QString::fromUtf8(info_->username.c_str()));
    map.insert("password", QString::fromUtf8(info_->password.c_str()));
    list << map;
    return list;
}

Q_INVOKABLE void Receiver::setSettingsData(QVariantList l) //to qml file
{
    QVariantMap m = l.at(0).toMap();
    info_->mail     = m["mail"].toString().toStdString();
    info_->server   = m["server"].toString().toStdString();
    info_->name     = m["name"].toString().toStdString();
    info_->username = m["username"].toString().toStdString();
    info_->password = m["password"].toString().toStdString();
    connect(info_);
}

Q_INVOKABLE void Receiver::getSettingsFromListView()
{
    emit getSettings();
}

Q_INVOKABLE void Receiver::setSettingsToListView()
{
    emit setSettings();
}

//---
void Receiver::emitReceive()
{
    emit receive();
    //emit setSettings();
}

////////////////////////////////////////
////////////////////////////////////////

//отправка сообщения на сервер
void Receiver::send_socket(std::string s)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.writeRawData(s.c_str(),s.length());
    // подготовка данных для записи //
    socket_->write(data);
    socket_->waitForBytesWritten();
    if (logout_)
        (*log_) << s;
}

const int BUFSIZE = 4096;
//прочитать сообщение из сокета
void Receiver::read_socket()
{
    char buf[BUFSIZE+1];
    QDataStream in(socket_);
    socket_->waitForReadyRead();
    int index = in.readRawData(buf,BUFSIZE);
    buf[index] = 0;
    if (logout_)
        (*log_) << buf;
}

//прочитать из сокета, проверяя пароль
void Receiver::read_socket_with_pass_check()
{
    char *buf = new char[BUFSIZE+1];
    unsigned int len;
    QDataStream in(socket_);
    socket_->waitForReadyRead();
    int index = in.readRawData(buf,BUFSIZE);
    buf[index] = 0;
    std::string bu = buf;
    if (bu.find("OK")==std::string::npos)
    {
        //выход
        end();
        (*log_) << "Wrong password!";
        throw WrongPassword((*log_).str());
    }
    if (logout_)
        (*log_) << buf;
}

//---
void Receiver::read_flag(bool *a)
{
    char buf[BUFSIZE+1];
    QDataStream in(socket_);
    socket_->waitForReadyRead();
    in.readRawData(buf,BUFSIZE);
    std::istringstream imem(buf);
    std::string word;
    while (imem.good())
    {
        imem >> word;
        if (word=="SEARCH") break;
    }
    if (!imem.good()) return;
    int num;
    while (1)
    {
        imem >> num;
        //std::cerr << num << ' ';
        if (!imem.good()) return;
        a[num-1] = 1;
    }
}

//конец соединения
void Receiver::end()
{
    send_socket(std::string("QUIT")+END);
    read_socket();
    socket_->abort();
}

////////////////////////////////////////
//количество сообщений в ящике
int Receiver::countMessages(std::string mailbox)
{
    //ask / спрашиваем
    send_socket(std::string("$ STATUS ") + mailbox + " (messages)"+END);

    char buf[BUFSIZE+1];
    QDataStream in(socket_);
    socket_->waitForReadyRead();
    in.readRawData(buf,BUFSIZE);

    std::string str = buf;
    std::istringstream imem(str);
    std::string word;
    int count=0;
    const int MAXN=100;

    //находим начало
    while (1)
    {
        imem >> word;
        if (word.find("MESSAGES")!=std::string::npos) break;
        if (count++==MAXN) break;
    }

    //читаем количество
    int nMessages;
    if (count==MAXN-1)
        nMessages = 0;
    else
        imem >> nMessages;

    return nMessages;
}

//прочитать заголовочную информацию
std::string Receiver::readHeader()
{
    char buf[BUFSIZE+1];
    QDataStream in(socket_);
    socket_->waitForReadyRead();

    std::string str;
    while (!in.atEnd())
    {
        in.readRawData(buf,BUFSIZE);
        str+=buf;
    }

    if (logout_)
    {
        (*log_) << str;
    }

    return fetchSubj(str);
}

//прочитать сообщение
std::string Receiver::readContnt()
{
    QDataStream in(socket_);
    char buf[BUFSIZE+1];
    std::string contnt;
    socket_->waitForReadyRead();

    in.readRawData(buf,BUFSIZE);
    contnt+=buf;

    int index1 = contnt.find("{")+1;
    int index2 = contnt.find("}");
    std::istringstream imem(contnt.substr(index1,index2-index1));

    int nSymbols;
    imem >> nSymbols;

    while (contnt.length()<nSymbols+index2+1)
    {
        in.readRawData(buf,BUFSIZE);
        contnt+=buf;
    }

    if (logout_)
        (*log_) << contnt;

    std::cerr << "symbols: " << nSymbols << std::endl;
    std::cerr << "symbols2: " << contnt.length() << std::endl;


    return contnt.substr(index2+2,nSymbols);
}

//достать именно заголовок
std::string Receiver::fetchSubj(std::string& s)
{
    int index1 = s.find("Subject:")+9;
    int index2 = s.length();
    std::istringstream imem(s.substr(index1,index2-index1));
    char *buf = new char[index2-index1];
    std::string header;
    while (1)
    {
        imem.getline(buf,index2-index1);
        if (strlen(buf)==1 && buf[0]==13) break;
        std::string newstr = buf;
        newstr = newstr.substr(0,newstr.length()-1);
        if (header!="") header+='\n';
        header += newstr;
    }
    return header;
}

