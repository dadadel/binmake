/*
 * BinStream.h
 *
 *  Created on: 2 janv. 2017
 *  Author: Adel Daouzli
 *  License: MIT License
 */

#ifndef BINSTREAM_H_
#define BINSTREAM_H_

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

class BinStream
{
private:
    std::stringstream m_input; // the input string to convert to binary
    std::vector<char> m_output; // the output binary data to generate

    endianess_t curr_endianess;
    type_t curr_numbers;
    type_t curr_type;

    bool m_verbose;
public:
	BinStream();
	~BinStream();
	bool get_binary(std::vector<char>& output);
	void add_element(const std::string & element);
	void proceed_element(const std::string & element);
	void update_bin_output(const type_t stype, const endianess_t etype, const std::string& s);

	void bb_log(std::string msg);
};

#endif /* BINSTREAM_H_ */
