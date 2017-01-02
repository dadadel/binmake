//============================================================================
// Name        : util.cpp
// Author      : Adel Daouzli
// Version     :
// Copyright   : MIT License
// Description : Useful functions header
//============================================================================


#ifndef UTILS_H_
#define UTILS_H_

#include <string>

#define PREFIX_NUMBER_DECIMAL "%d"
#define PREFIX_NUMBER_HEXADECIMAL "%x"
#define PREFIX_NUMBER_BINARY "%b"
#define PREFIX_NUMBER_OCTAL "%o"

#define MAX_U32b_VALUE 0x00000000FFFFFFFFUL
#define MAX_U16b_VALUE 0x000000000000FFFFUL
#define MAX_U8b_VALUE  0x00000000000000FFUL

bool starts_with(const std::string &s, const std::string &prefix);
std::string& lstrip(std::string& s);
std::string& rstrip(std::string& s);
std::string& strip(std::string& s);
bool endswith(const std::string &str, const std::string &suffix);

class ExceptionBBInterpretation
{
};


#endif /* UTILS_H_ */
