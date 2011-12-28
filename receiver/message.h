/*****************************************
Файл message.h
Информация о сообщениях, пока что можно
получить только заголовок и тело сообщения /
Information about the message
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
#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <string>
class Address; //{почтовый адрес / mail address}
class Date; //{дата и время / date and time}
class Message
{
public:
    Address *sender; //отправитель
    std::string header; //заголовок
    std::string contnt; //тело сообщения
    Date* date; //дата отправки
    bool bRecent; //{недавнее ли это сообщение}
};
#endif
