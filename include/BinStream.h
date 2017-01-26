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

#include "bs_data.h"
#include "bs_exception.h"

namespace BS
{
    class BinStream
    {
    private:
        std::stringstream m_input; // the input string to convert to binary
        std::vector<char> m_output; // the output binary data to generate

        endianess_t m_curr_endianess;
        type_t m_curr_numbers;
        int m_curr_size;

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

        void set_verbosity(bool verbose);

        bool input_ready(void) const;
        bool output_ready(void) const;
        bool get_output(std::vector<char>& output) const;
        size_t size(void) const;

        char operator[](const size_t index) const;

        // Stream operators
        BinStream& operator<<(const std::istream & s);
        BinStream& operator<<(const std::ifstream & f);
        BinStream& operator<<(const std::stringstream & desc);
        BinStream& operator<<(const std::string & desc);
        BinStream& operator>>(std::ofstream & f);
        BinStream& operator>>(std::vector<char> & output);

        friend std::ostream& operator<<(std::ostream& stream, const BinStream& bin_stream);
        friend std::istream& operator>>(std::istream& stream, BinStream& bin_stream);

        // Low-level functions for parsing input and generating output
        bool update_internal(const std::string & element);
        void proceed_input(const std::string & element);
        void workflow(const std::string & element);

        void bs_log(std::string msg);
        void bs_error(std::string msg);
    };
}

#endif /* BINSTREAM_H_ */
