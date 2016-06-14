//============================================================================
// Name        : builtbin.h
// Author      : Adel Daouzli
// Version     :
// Copyright   : GPL3
// Description : Built binary file header
//============================================================================

#ifndef BINBUIDLER_H_
#define BINBUIDLER_H_

#include <sstream>
#include <string>
#include <vector>

typedef enum
{
    t_num_hexadecimal,
    t_num_decimal,
    t_num_octal,
    t_num_boolean,
    t_string,
    t_none
} type_t;

typedef enum
{
    little_endian,
    big_endian
} endianess_t;

class BinBuidler
{
private:
    std::stringstream m_input; // the input string to convert to binary
    std::vector<char> m_output; // the output binary data to generate
    bool m_input_available;
    bool m_output_generated;
    bool m_output_updated;

    void parse_data(void);
    void add_to_bin_output(const type_t stype, const endianess_t etype, const std::string &s);

public:
    BinBuidler();
    BinBuidler(const BinBuidler &b);
    BinBuidler(std::string &s);
    BinBuidler(std::stringstream &s);
    virtual ~BinBuidler();
    void set_input(const std::stringstream &input);
    void set_input(const std::string &input);
    bool buildbin(const std::stringstream &input);
    bool buildbin(const std::string &input);
    bool buildbin(void);
    bool get_binary(std::vector<char> &output) const;
};

#endif /* BINBUIDLER_H_ */
