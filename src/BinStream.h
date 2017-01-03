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

#include "bs_exception.h"

namespace BS
{
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

        endianess_t m_curr_endianess;
        type_t m_curr_numbers;

        bool m_input_ready;
        bool m_output_ready;

        bool m_verbose;

    public:
        BinStream(bool verbose=false);
        BinStream(const BinStream& o);
        ~BinStream();

        void reset(void);
        void reset_modes(void);
        void reset_output(void);
        void reset_input(void);

        bool input_ready(void) const;
        bool output_ready(void) const;
        bool get_output(std::vector<char>& output) const;

        // Stream operators
        BinStream& operator<<(const std::ifstream & f);
        BinStream& operator<<(const std::stringstream & desc);
        BinStream& operator<<(const std::string & desc);
        BinStream& operator>>(std::ofstream & f);
        BinStream& operator>>(std::vector<char> & output);

        // Low-level functions for parsing input and generating output
        void parse_input(const std::string & element);
        void proceed_element(const std::string & element);
        void update_bin_output(const type_t stype, const endianess_t etype, const std::string& s);

        void bs_log(std::string msg);
    };
}

#endif /* BINSTREAM_H_ */
