/*****************************************
Файл Message.h
Информация о сообщениях, пока что можно
получить только заголовок и тело сообщения /
Information about the message
*******************************************/
#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <string>
class Address;
class Date;
class Message
{
public:
    Address *sender; //отправитель
    std::string header; //заголовок
    std::string contnt; //тело сообщения
    Date* date; //дата отправки
    bool bRecent;
};
#endif
