//u82w1251.cpp
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
#include "u82w1251.hpp"
#include <string>
#include <iostream>
//перевести строку в cp1251 / translate to cp1251
std::string utf8_2_win1251(const std::string& s)
{
    //результат обработки s
    std::string result;
    //устанавливается в 1 или 2, когда a>127 / set to 1 or 2, then a is
    int status = 0; //larger than 127


    for (int i=0; i<(int)s.length(); i++)
    {
        unsigned char a = s[i];

        if (status)
        {
            if (status==1)
            {
                if (a==129)
                    result += (unsigned char)(168);
                else
                    result += (a+48);
            }
            else if (status==2)
            {
                if (a==209 || a==145)
                    result += (unsigned char)(184);
                else
                    result += (a+112);
            }
            status = 0;
        }
        else
        {
            if (a<=127)
                result += a;
            else if (a==208)
                status = 1;
            else if (a==209)
                status = 2;
        }
    }

    return result;
}

std::string win12512utf8(std::string& s)
{
    std::string result;

    for (int i=0; i<(int)s.length(); i++)
    {
        char a = s[i];
        if (a>0)
            result+=a;
        else if (a==-88 || a==-72)
        {
            if (a==-88)
                result+=(unsigned char)(208);
            else if (a==-72)
                result+=(unsigned char)(209);
            result+=(a+217);
        }
        else if (a>=-64 && a<=-17)
            result=(result+(char)(208))+(char)(a+208);
        else if (a>=-16 && a<=-1)
            result=(result+(char)(209))+(char)(a+144);
    }
    return result;
}
