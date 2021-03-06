#include "receiver.h"
#include "message.h" //информация о сообщении
#include "info.h" //о клиенте imap
#include <QVariantMap>

//base64_encode()
#include "base64.h"
#include "quoted.h"
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

    ready_ = 1;

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
    if (!ready_) return QVariantList();

    std::string mailbox = "INBOX"; //почтовый ящик по умолчанию

    int nMessages = countMessages(mailbox); //количество сообщений

    //выбор ящика / select mailbox
    send_socket(std::string("$ SELECT ") + mailbox +END);
    read_socket();

    /*std::ostringstream omem;
    omem << "$ SEARCH UNDELETED" << END;
    send_socket(omem.str());
    bool *undeleted = new bool[nMessages];
    memset(undeleted,0,nMessages);
    std::cerr << "================1=============\n" << std::endl;
    read_flag(undeleted);*/

    /*std::ostringstream omem2;
    omem2 << "$ SEARCH RECENT" << END;
    send_socket(omem2.str());
    bool *recent = new bool[nMessages];
    memset(recent,0,nMessages);
    std::cerr << "================r=============\n" << std::endl;
    read_flag(recent);*/

    QVariantList mess; //список сообщений
    std::cerr << nMessages << std::endl;
    for (int i=nMessages-1; i>=0 && i>=nMessages-21; i--) //получить сообщения / get messages
    {
        //if (!undeleted[i]) continue;
        //if (!recent[i]) continue;
        Message *mes = new Message(); //новое сообщение

        //получение заголовка
        std::ostringstream omem;
        omem << "$ FETCH " << i+1
             << " (body[header.fields (from subject date)])" << END;
        send_socket(omem.str());
        mes->header = readHeader();

        std::ostringstream omem2;
        omem2 << "$ FETCH " << i+1
             << " (flags)" << END;
        send_socket(omem2.str());
        bool bDeleted = 0;
        readIsDeleted(bDeleted);
        if (bDeleted) continue;

        int pos1 = 0, pos2 = 0;

        while (1)
        {
            int pos = mes->header.find("\r\n");
            if (pos==(int)std::string::npos)
            {
                pos = mes->header.find("\n");
                if (pos==(int)std::string::npos)
                    break;
                mes->header.replace(pos,2,"");
            }
            else
            {
                mes->header.replace(pos,3,"");
            }
        }

        while (1)
        {
            pos1 = (int)mes->header.find("=?",pos1);
            if (pos1==(int)std::string::npos) break;
            int tpos = (int)mes->header.find("?",pos1+2);
            if (mes->header[tpos+2]!='?') break;
            pos2 = (int)mes->header.find("?=",tpos+3);
            if (pos2==(int)std::string::npos) break;
            std::cerr << i << ") was " << mes->header << std::endl;
            if (mes->header[tpos+1]=='b' || mes->header[tpos+1]=='B')
                mes->header.replace(pos1,pos2-pos1+2,
                    base64_decode(mes->header.substr(tpos+3,pos2-tpos-3)));
            if (mes->header[tpos+1]=='q' || mes->header[tpos+1]=='Q')
                mes->header.replace(pos1,pos2-pos1+2,
                    quotedDecode(mes->header.substr(tpos+3,pos2-tpos-3)));
            std::cerr << i << ") bec " << mes->header << std::endl;
        }

        //получение тела / get a body
        /*std::ostringstream omem2;
        omem2 << "$ FETCH " << i+1 << " BODY[text]" << END;
        send_socket(omem2.str());
        mes->contnt = readContnt();
        std::cerr << i << ") contnt " << mes->contnt << std::endl;*/

        //элемент
        QVariantMap map;
        map.insert("header", QString::fromUtf8(mes->header.c_str()));
        //map.insert("content",QString::fromStdString(mes->contnt));
        mess << map;
    }

    //delete[] undeleted;
    //delete[] recent;
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
    std::string str = readSocketAnswer();
    if (logout_)
        (*log_) << str;
}

void Receiver::readIsDeleted(bool& bDel)
{
    std::string str = readSocketAnswer();
    if (str.find("\\Deleted")!=std::string::npos)
        bDel = 1;
    if (logout_)
        (*log_) << str;
}

//прочитать из сокета, проверяя пароль
void Receiver::read_socket_with_pass_check()
{
    std::string str = readSocketAnswer();
    if (str.find("OK")==std::string::npos)
    {
        //выход
        end();
        (*log_) << "Wrong password!";
        throw WrongPassword((*log_).str());
    }
    if (logout_)
        (*log_) << str;
}

//---
std::string Receiver::readSocketAnswer()
{
    std::string str;
    QDataStream in(socket_);
    char buf[BUFSIZE+1];
    memset(buf,0,BUFSIZE);
    socket_->waitForReadyRead(timeout);
    int size = in.readRawData(buf,BUFSIZE);
    if (size==-1)
        throw Unconnected();
    if (size==0)
        return "";

    buf[size] = 0;

    str+=buf;

    return str;
}

std::string Receiver::readBigSocketAnswer()
{
    std::string str;
    QDataStream in(socket_);
    char buf[BUFSIZE+1];
    while (1)
    {
        memset(buf,0,BUFSIZE);
        socket_->waitForReadyRead(timeout);
        std::cerr << "bbbb{ " << socket_->bytesAvailable() << "}\n";
        int size = in.readRawData(buf,BUFSIZE);
        if (size==-1)
            throw Unconnected();
        if (size==0)
            break;

        buf[size] = 0;
        str+=buf;
    }

    return str;
}

void Receiver::read_flag(bool *a)
{
    std::string str = readBigSocketAnswer();

    if (logout_)
        (*log_) << str << std::endl;
    std::istringstream imem(str.c_str());
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

    std::string str = readSocketAnswer();
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
    std::string str = readSocketAnswer();

    if (logout_)
        (*log_) << str;

    return fetchSubj(str);
}

//прочитать сообщение
std::string Receiver::readContnt()
{
    std::string contnt = readBigSocketAnswer();

    int index1 = contnt.find("{")+1;
    int index2 = contnt.find("}");
    std::istringstream imem(contnt.substr(index1,index2-index1));

    int nSymbols;
    imem >> nSymbols;

    if (logout_)
        (*log_) << contnt;

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

