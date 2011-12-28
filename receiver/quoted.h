/*****************************************
Файл quoted.h
Функции для кодировки из/в quoted-printable (флаг ?Q? в строке заголовка) /
functions for qouted-printable encoding/decoding
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

#ifndef QUOTED_H
#define QUOTED_H
#include <string>

std::string quotedEncode(unsigned char const*); //кодирование
std::string quotedDecode(std::string const& s); //декодирование

#endif // QUOTED_H
