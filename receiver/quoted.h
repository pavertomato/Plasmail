#ifndef QUOTED_H
#define QUOTED_H
#include <string>

std::string quotedEncode(unsigned char const*);
std::string quotedDecode(std::string const& s);

#endif // QUOTED_H
