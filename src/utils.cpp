//============================================================================
// Name        : util.cpp
// Author      : Adel Daouzli
// Version     :
// Copyright   : MIT License
// Description : Useful functions
//============================================================================

#include "utils.h"


/**
 * @brief Check if a string s starts with a string prefix
 *
 * @param s the string to check
 * @param prefix the prefix to check
 *
 * @return true if s starts with prefix else false
 */
bool BS::starts_with(const std::string &s, const std::string &prefix)
{
    return ((prefix.size() <= s.size()) &&
            std::equal(prefix.begin(), prefix.end(), s.begin()));
}

/**
 * @brief Check if a string s ends with a string suffix
 *
 * @param s the string to check
 * @param suffix the prefix to check
 *
 * @return true if s ends with prefix else false
 */
bool BS::endswith(const std::string &s, const std::string &suffix)
{
    return ((s.size() >= suffix.size()) &&
            (s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0));
}

/**
 * @brief Remove trailing spaces of a string.
 * Note: The provided string will be updated and will be returned

 * @param s a reference to the string to strip

 * @return a reference to the stripped string
 */
std::string& BS::lstrip(std::string& s)
{
    size_t i;
    for(i = 0; i < s.size() && isspace(s[i]); ++i);
    return s.erase(0, i);
}

/**
 * @brief Remove ending spaces of a string.
 * Note: The provided string will be updated and will be returned

 * @param s a reference to the string to strip

 * @return a reference to the stripped string
 */
std::string& BS::rstrip(std::string& s)
{
    size_t i;
    for(i = s.size(); i > 0 && isspace(s[i - 1]); --i);
    return s.erase(i, s.size());
}

/**
 * @brief Remove trailing and ending spaces of a string.
 * Note: The provided string will be updated and will be returned

 * @param s a reference to the string to strip

 * @return a reference to the stripped string
 */
std::string& BS::strip(std::string& s)
{
    return rstrip(lstrip(s));
}
