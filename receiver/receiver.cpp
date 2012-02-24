//receiver.cpp
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
#include "receiver.h"
#include "message.h" //информация о сообщении
#include "info.h" //о клиенте imap
#include <QVariantMap> //определение класса для множества настроек
//
#include "quoted.h"

//utf8 to windows1251
#include "u82w1251.hpp"
#include "messagesettings.h" //параметры тела сообщения
#include <sstream> //обработка чисел в строках
//std::cerr
#include <iostream>
//memcpy()
#include <string.h>
//exit()
#include <stdlib.h>
#include <QtNetwork/QTcpSocket> //сокет
#include <QDataStream> //поток для сокета
//конец строки / end of line
std::string END = "\r\n";

//соединиться с сервером / connect to server
void Receiver::connect(Info* info)
{
    ready_ = 0; //мы пока не готовы
    info_ = info; //сохранение ссылки на информацию

    //установливаем log
    log_ = new std::ostringstream();
    logout_ = 1; //вывод лога нужен

    //копируем информацию о нас / it's all about us
    mail_ = info->mail;
    server_ = info->server;
    username_ = info->username;
    password_ = info->password;
    name_ = info->name;

    //установить порт
    int i;
    for (i=server_.length(); i>0; i--)
        if (server_[i]==':')
            break;
    int port;
    if (i!=0)
    {
        std::istringstream istm(server_.substr(i+1));
        istm >> port;
    }
    else
    {
        i = server_.length();
        port=143; //стандартный порт
    }

    socket_ = new QTcpSocket(this);
    socket_->connectToHost(QString::fromStdString(server_.substr(0,i)),
                           port);

    //ждём, когда соединимся, вернее "если соединимся"
    if (!socket_->waitForConnected(timeout)) return;

    read_socket(); //привет
    //меня зовут Plasmail
    send_socket(std::string("$ LOGIN ")+username_+" "+password_+END);
    read_socket_with_pass_check(); //дай-ка мне паспорт

    ready_ = 1; //соединены
}

//вернуть список сообщений в qml / return a list of messages to qml file
Q_INVOKABLE QVariantList Receiver::messages()
{
    if (!ready_) return QVariantList(); //если нет смысла продолжать

    send_socket(std::string("$ LIST * *") +END);
    readBoxes();

    for (int i=0; i<boxesList_.size(); i++)
        if (boxesList_[i]=="INBOX")
        {
            currentBox_ = i;
            break;
        }

    //почтовый ящик
    std::string mailbox = boxesList_[currentBox_];

    int nMessages = countMessages(mailbox); //количество сообщений

    //выбор ящика / select mailbox
    send_socket(std::string("$ SELECT ") + mailbox +END);
    read_socket();

    QVariantList mess; //список сообщений
    //получить сообщения в количестве до MAXNMESSAGES штук / get messages
    for (int i=nMessages-1; i>=0 && i>=nMessages-MAXNMESSAGES; i--)
    {
        Message *mes = new Message(); //новое сообщение

        //получение заголовка / get header
        std::ostringstream omem;
        omem << "$ FETCH " << i+1
             << " (body[header.fields (from subject date)])" << END;
        send_socket(omem.str());
        mes->header = readHeader();

        //узнать удалены ли сообщения / get is deleted
        omem.str("");
        omem << "$ FETCH " << i+1
             << " (FLAGS)" << END;
        send_socket(omem.str());
        bool bDeleted = 0;
        readIsDeleted(bDeleted);
        if (bDeleted) continue; //сообщение удалено, идём дальше

        //узнать параметры тела сообщения
        omem.str("");
        omem << "$ FETCH " << i+1
             << " (BODYSTRUCTURE)" << END;
        send_socket(omem.str());
        MessageSettings messageSettings;
        readMessageSettings(messageSettings);
        if (bDeleted) continue; //сообщение удалено, идём дальше

        //получение тела / get a body
        omem.str("");
        omem << "$ FETCH " << i+1 << " BODY[text]" << END;
        send_socket(omem.str());
        mes->contnt = readContnt();

        //поиск разрывов в заголовке / find gaps
        while (1)
        {
            int pos = mes->header.find("\r\n");
            if (pos==(int)std::string::npos)
            {
                pos = mes->header.find("\n");
                if (pos==(int)std::string::npos)
                    break;
                mes->header.replace(pos,2,"");
                continue;
            }
            mes->header.replace(pos,3,"");
        }

        /*поиск кодирующих шаблонов (таких как =?windows-1251?Q? и
        =?utf-8?B?, где первая строка означает набор символов, второй коди-
        ровку base64 или qouted-encoded), эти шаблоны включают в себя стро-
        ку, заканчивающуюся на ?=*/
        int pos1 = 0, pos2 = 0;
        int encoding=0; //0 -- unicode, 1 -- win1251, 2 -- koi8-r
        while (1)
        {
            //начало
            pos1 = (int)mes->header.find("=?",pos1);
            if (pos1==(int)std::string::npos) break; //нету больше шаблонов

            //символ отделяющий кодировку
            int tpos = (int)mes->header.find("?",pos1+2);
            //значит это не шаблон, мы такого не знаем
            if (mes->header[tpos+2]!='?') break;

            //конец шаблона
            pos2 = (int)mes->header.find("?=",tpos+3);
            if (pos2==(int)std::string::npos) break; //его нет

            std::string sEncoding = mes->header.substr(pos1+2,tpos-pos1-2);
            if (QString::compare(QString::fromStdString(sEncoding),
                                 "windows-1251",Qt::CaseInsensitive)==0)
                encoding = 1;
            else if (QString::compare(QString::fromStdString(sEncoding),
                                 "koi8-r",Qt::CaseInsensitive)==0)
                encoding = 2;
            //проверка на не unicode
            else if (QString::compare(QString::fromStdString(sEncoding),
                                 "utf-8",Qt::CaseInsensitive))
                encoding = -1;

            std::string newstr = mes->header.substr(tpos+3,pos2-tpos-3);
            //замена base64
            if (mes->header[tpos+1]=='b' || mes->header[tpos+1]=='B')
            {
                newstr = std::string(QByteArray::fromBase64(
                                QByteArray(newstr.c_str())).data());
            }
            //замена quoted-printable
            else if (mes->header[tpos+1]=='q' || mes->header[tpos+1]=='Q')
                newstr = quotedDecode(newstr);

            //перекодирование куска
            if (encoding)
                encode(newstr,encoding,sEncoding);

            mes->header.replace(pos1,pos2-pos1+2,newstr);
        }

        if (messageSettings.bBase64)
            mes->contnt = std::string(QByteArray::fromBase64(
                QByteArray(mes->contnt.c_str())).data());
        //поиск разрывов в теле сообщения
        while (1)
        {
            int pos = mes->contnt.find("\r");
            if (pos==(int)std::string::npos)
                break;
            mes->contnt.replace(pos,1,"");
        }

        encode(mes->contnt,messageSettings.encoding,
               messageSettings.sEncoding);

        //элемент
        QVariantMap map;
        map.insert("header", QString::fromUtf8(mes->header.c_str()));
        map.insert("body", QString::fromUtf8(mes->contnt.c_str()));
        mess << map;
    }

    return mess;
}

//графический файл получает параметры / set to qml
Q_INVOKABLE QVariantMap Receiver::settings()
{
    QVariantMap map;
    map.insert("mail",     QString::fromUtf8(info_->mail.c_str()));
    map.insert("server",   QString::fromUtf8(info_->server.c_str()));
    map.insert("name",     QString::fromUtf8(info_->name.c_str()));
    map.insert("username", QString::fromUtf8(info_->username.c_str()));
    map.insert("password", QString::fromUtf8(info_->password.c_str()));
    return map;
}

//получение параметров из графичско / get to qml
Q_INVOKABLE void Receiver::setSettingsData(QVariantMap m) //to qml file
{
    info_->mail     = m["mail"].toString().toStdString();
    info_->server   = m["server"].toString().toStdString();
    info_->name     = m["name"].toString().toUtf8().constData();
    info_->username = m["username"].toString().toStdString();
    info_->password = m["password"].toString().toStdString();
    connect(info_);
    emitReceive();
}

Q_INVOKABLE void Receiver::getSettingsFromListView()
{
    emit getSettings();
}

Q_INVOKABLE void Receiver::setSettingsToListView()
{
    emit setSettings();
}

void Receiver::emitReceive()
{
    emit receive();
}

void Receiver::emitChangeMode()
{
    emit changeMode();
}

////////////////////////////////////////
////////////////////////////////////////

//отправка сообщения на сервер / s -- string message
void Receiver::send_socket(std::string s)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.writeRawData(s.c_str(),s.length());
    // подготовка данных для записи //
    socket_->write(data);
    socket_->waitForBytesWritten(); //ожидание
    if (logout_) //сохранение выводимой информации
        (*log_) << s;
}

//прочитать сообщение из сокета, не выполняя каких-либо действий с
void Receiver::read_socket() //полученной информацией
{
    std::string str = readSocketAnswer();
    if (logout_) //сохранение полученной информации
        (*log_) << str;
}

//прочитать, является ли сообщение удалённым
void Receiver::readIsDeleted(bool& bDel)
{
    std::string str = readSocketAnswer();
    if (str.find("\\Deleted")!=std::string::npos)
        bDel = 1;
    if (logout_)
        (*log_) << str;
}

void Receiver::readMessageSettings(MessageSettings &set)
{
    std::string str = readSocketAnswer();
    if (str.find("\"base64\"")!=std::string::npos)
        set.bBase64 = 1;
    int pos1 = str.find("\"charset\"")+11;
    int pos2 = str.find('"',pos1);
    set.sEncoding = str.substr(pos1,pos2-pos1);

    if (QString::compare(QString::fromStdString(set.sEncoding),
                         "windows-1251",Qt::CaseInsensitive)==0)
        set.encoding = 1;
    else if (QString::compare(QString::fromStdString(set.sEncoding),
                         "koi8-r",Qt::CaseInsensitive)==0)
        set.encoding = 2;
    //проверка на не unicode
    else if (QString::compare(QString::fromStdString(set.sEncoding),
                         "utf-8",Qt::CaseInsensitive))
        set.encoding = -1;
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

void Receiver::readBoxes()
{
    std::string str = readSocketAnswer();
    std::istringstream imem(str.c_str());
    if (str.find("OK")==std::string::npos)
    {
        boxesList_.push_back("INBOX");
    }
    else
    {
        while (1)
        {
            char line[1024+1];
            imem.getline(line,sizeof(line)-1);
            if (!imem.good()) break;
            std::string sline = line;
            if (sline.find("OK")!=std::string::npos) break;
            int index2 = sline.find_last_of('"');
            if ((std::string::size_type)index2==std::string::npos) break;
            int index1 = sline.find_last_of('"',index2-1);
            boxesList_.push_back(sline.substr(index1+1,index2-index1-1));
        }
    }
    if (logout_)
        (*log_) << str;

}

//функция для чтения строки, возвращаемой сокетом
std::string Receiver::readSocketAnswer()
{
    QDataStream in(socket_); //соединяем сокет с потоком
    //ждём, пока мы не сможем начать чтение из потока
    socket_->waitForReadyRead(timeout);
    std::string str;
    while (!in.atEnd())
    {
        int size = socket_->bytesAvailable();
        char *buf = new char[size+1];
        memset(buf,0,size);
        size = in.readRawData(buf,size);
        if (size==-1)
            throw Unconnected(); //мы не соединены
        if (size==0)
            return ""; //нам ничего не ответили

        buf[size] = 0; //последний символ -- нулевой
        str+=buf;
        //socket_->waitForReadyRead(timeout);
    }
    return str;
}

//функция для чтения строки, возвращаемой сокетом
std::string Receiver::readContntSocketAnswer()
{
    QDataStream in(socket_); //соединяем сокет с потоком
    //ждём, пока мы не сможем начать чтение из потока
    socket_->waitForReadyRead(timeout);
    std::string str;
    while (!in.atEnd())
    {
        int size = socket_->bytesAvailable();
        char *buf = new char[size+1];
        memset(buf,0,size);
        size = in.readRawData(buf,size);
        if (size==-1)
            throw Unconnected(); //мы не соединены
        if (size==0)
            return ""; //нам ничего не ответили

        buf[size] = 0; //последний символ -- нулевой
        str+=buf;

        int index1 = str.find("{")+1;
        int index2 = str.find("}");
        std::istringstream imem(str.substr(index1,index2-index1));

        int nSymbols;
        imem >> nSymbols;
        int index3 = str.find("$ OK");
        //std::cerr << nSymbols << "   size: " << size;
        //std::cerr << " size2: " << index3-index2-6 << str.substr(index2+1,index3-index2-6) << std::endl;
        if (index3-index2-6<nSymbols)
            socket_->waitForReadyRead(timeout);
    }
    return str;
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
    std::string contnt = readContntSocketAnswer();

    //в промежутке [index1,index2) находится количество символов
    int index1 = contnt.find("{")+1;
    int index2 = contnt.find("}");
    std::istringstream imem(contnt.substr(index1,index2-index1));

    int nSymbols;
    imem >> nSymbols;

    if (logout_)
        (*log_) << contnt;

    return contnt.substr(index2+2,nSymbols);
}

//достать именно заголовок / s -- string, not subj; subject is returned
std::string Receiver::fetchSubj(std::string& s)
{
    int index1 = s.find("Subject:")+9; //начало заголовка
    int index2;
    index2 = s.find("From:",index1); //конец

    //нахождение конца заголовка
    if (index2==(int)std::string::npos)
    {
        index2 = s.find("Date:",index1);
        if (index2==(int)std::string::npos)
            index2 = s.length();
    }

    std::istringstream imem(s.substr(index1,index2-index1));
    char *buf = new char[index2-index1+1];
    std::string header;
    //чтение многострочного заголовка
    while (1)
    {
        imem.getline(buf,index2-index1);
        if ((strlen(buf)==1 && buf[0]==13) || strlen(buf)==0) break;
        index1+=strlen(buf);
        std::string newstr = buf;
        newstr = newstr.substr(0,newstr.length()-1);
        if (header!="") header+='\n';
        header += newstr;
    }
    delete[] buf;
    return header;
}
