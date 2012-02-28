/*****
  Параметры тела сообщения
  copyright (c) 2012 Лежнин Евгений
  */////
#ifndef MESSAGESETTINGS_H
#define MESSAGESETTINGS_H

#include <string>
class MessageSettings
{
public:
    MessageSettings();
    bool bBase64;
    bool bQuotedPrintable;
    int encoding;
    std::string sEncoding;
};

#endif // MESSAGESETTINGS_H
