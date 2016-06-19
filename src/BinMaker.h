//============================================================================
// Name        : builtbin.h
// Author      : Adel Daouzli
// Version     :
// Copyright   : GPL3
// Description : Built binary file header
//============================================================================

#ifndef BinMaker_H_
#define BinMaker_H_

#include <sstream>
#include <string>
#include <vector>

typedef enum
{
    t_num_hexadecimal,
    t_num_decimal,
    t_num_octal,
    t_num_binary,
    t_string,
    t_none
} type_t;

typedef enum
{
    little_endian,
    big_endian
} endianess_t;

class BinMaker
{
private:
    std::stringstream m_input; // the input string to convert to binary
    std::vector<char> m_output; // the output binary data to generate
    bool m_input_available;
    bool m_output_generated;
    bool m_output_updated;
    bool m_verbose;

    void parse_data(void);
    void add_to_bin_output(const type_t stype, const endianess_t etype, const std::string &s);
    void bb_log(std::string msg);

public:
    BinMaker();
    BinMaker(const BinMaker &b);
    BinMaker(std::string &s);
    BinMaker(std::stringstream &s);
    virtual ~BinMaker();
    void set_verbosity(bool activation);
    void set_input(const std::stringstream &input);
    void set_input(const std::string &input);
    /*bool buildbin(const std::stringstream &input);
    bool buildbin(const std::string &input);*/
    bool buildbin(void);
    bool get_binary(std::vector<char> &output);
};

#endif /* BinMaker_H_ */
