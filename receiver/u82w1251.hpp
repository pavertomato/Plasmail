/*****************************************
Файл u82w1251.hpp
функции для перевода строк из utf8 в windows1251{ и наоборот} /
functions for translating from utf8 to win1251
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
#ifndef U82W1251H
#define U82W1251H
#include <string>
//перевести строку в cp1251 / translate to cp1251
std::string utf8_2_win1251(const std::string& s);
std::string win12512utf8(std::string&);
#endif
