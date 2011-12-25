#include "quoted.h"
#include <sstream> //обработка чисел в строках
#include <iomanip>

//std::string qoutedEncode(unsigned char const*);

char toByte(std::string const &s, int p)
{
    int num;
    std::istringstream imem(s.substr(p+1,2));

    imem >> std::setbase(16) >> num;
    return (char)num;
}

std::string quotedDecode(std::string const& s)
{
    std::string ret;
    int currentPos = 0;
    while (true)
    {
        if (currentPos >= (int)s.length())
            break;

        char symb = s[currentPos];
        switch (symb)
        {
        case '=':
            ret+=toByte(s,currentPos);
            currentPos+=3;
            break;
        case '_':
            ret+=' ';
            currentPos++;
            break;
        default:
            ret+=s[currentPos++];
            break;
        }
    }
    return ret;
}
