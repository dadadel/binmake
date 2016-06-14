//============================================================================
// Name        : util.cpp
// Author      : Adel Daouzli
// Version     :
// Copyright   : GPL3
// Description : Useful functions header
//============================================================================


#ifndef UTILS_H_
#define UTILS_H_

#include <string>

bool starts_with(const std::string &s, const std::string &prefix);
std::string& lstrip(std::string& s);
std::string& rstrip(std::string& s);
std::string& strip(std::string& s);
bool endswith(const std::string &str, const std::string &suffix);



#endif /* UTILS_H_ */
