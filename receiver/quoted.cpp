//quoted.cpp
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
#include "quoted.h"
#include <sstream> //обработка чисел в строках
#include <iomanip> //setbase() в toByte()

/*обработка строки с шестнадцатиричным числом /
  parse char from little hexadecimal number string*/
char toByte(std::string const &s, int p)
{
    int num;
    std::istringstream imem(s.substr(p+1,2));

    imem >> std::setbase(16) >> num;
    return (char)num;
}

//раскодирование
std::string quotedDecode(std::string const& s)
{
    std::string ret; //возвращаемая строка
    int currentPos = 0; //текущая позиция в начальной строке
    while (1)
    {
        if (currentPos >= (int)s.length()) //строка закончилась
            break;

        char symb = s[currentPos]; //текущий символ / current symbol
        switch (symb)
        {
        case '=':
            ret+=toByte(s,currentPos); //обрабатываем символ
            currentPos+=3; //и идём дальше
            break;
        case '_':
            ret+=' '; //заменяем подчёркивание на пробел
            currentPos++;
            break;
        default:
            ret+=s[currentPos++]; //символ не закодирован, добавляем его
            break; //так
        }
    }
    return ret;
}
