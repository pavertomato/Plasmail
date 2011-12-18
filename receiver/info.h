/*****************************************
Файл info.h
Информация об отправителе /
information about sender
*******************************************/
#ifndef INFO_H_
#define INFO_H_
#include <string>
class Info
{
public:
    std::string mail, //почта
    server, //название сервера
    name, //имя
    username, //имя пользователя на этом сервере
    password; //пароль
};
#endif
