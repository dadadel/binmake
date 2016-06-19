/*
 * BinMaker.cpp
 *
 *  Created on: 14 juin 2016
 *      Author: adaouzli
 */

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "utils.h"

#include "BinMaker.h"


#define MAX_U32b_VALUE 0x00000000FFFFFFFFUL
#define MAX_U16b_VALUE 0x000000000000FFFFUL
#define MAX_U8b_VALUE  0x00000000000000FFUL

class ExceptionBBInterpretation
{
};



/**
 * @brief Default constructor
 */
BinMaker::BinMaker()
    : m_output(), m_input_available(false),
      m_output_generated(false), m_output_updated(false),
      m_verbose(false)
{
}

/**
 * @brief Copy constructor
 */
BinMaker::BinMaker(const BinMaker &b)
    : m_output(), m_input_available(b.m_input_available),
      m_output_generated(false), m_output_updated(false),
      m_verbose(false)
{
    m_input.str(b.m_input.str());
}

/**
 * @brief String copy constructor
 *
 * @param s the string to set as input text
 */
BinMaker::BinMaker(std::string& s)
: m_output(), m_input_available(false),
  m_output_generated(false), m_output_updated(false),
  m_verbose(false)
{
    m_input.str() = s;
}

/**
 * @brief String stream copy constructor
 *
 * @param s the string stream to set as input text
 */
BinMaker::BinMaker(std::stringstream& s)
: m_output(), m_input_available(false),
  m_output_generated(false), m_output_updated(false),
  m_verbose(false)
{
    m_input.str(s.str());
}

/**
 * @brief Destructor
 */
BinMaker::~BinMaker()
{
}

/**
 * @brief Build the output binary
 *
 * @return true if built the binary else false
 */
bool BinMaker::make_binary(void)
{
    if (m_input_available)
    {
        m_output.clear();
        parse_data();
        m_output_generated = true;
    }
    return m_output_generated;
}

void BinMaker::set_input(const std::stringstream& input)
{
    m_input.str(input.str());
    m_input_available = true;
}

void BinMaker::set_input(const std::string& input)
{
    m_input.str(input);
    m_input_available = true;
}

void BinMaker::set_verbosity(bool activation)
{
    m_verbose = activation;
}

/*TODO
bool BinMaker::make_binary(const std::stringstream& input)
{
}

bool BinMaker::make_binary(const std::string& input)
{
}
*/

/**
 * @brief Get the binary generated output.
 *
 * @param output will contain the output binary data
 *
 * @return true if the output was generated else false
 */
bool BinMaker::get_binary(std::vector<char>& output)
{
    if (!m_output_generated)
    {
        make_binary();
    }
    output = m_output;
    return m_output_generated;
}

void BinMaker::parse_data(void)
{
    std::string s;

    endianess_t endianess = little_endian;
    type_t numbers = t_num_hexadecimal;
    type_t curr_type = t_none;

    // parse input data
    while(getline (m_input, s))
    {
        strip(s);

        // ignore line

        if (starts_with(s, "#") || s.size() == 0)
        {
            bb_log("<ignore line>");
        }

        // change current endianness

        else if(s == "little-endian")
        {
            bb_log("<default endianess little-endian>");
            endianess = little_endian;
        }
        else if(s == "big-endian")
        {
            bb_log("<default endianess big-endian>");
            endianess = big_endian;
        }

        // change current number type

        else if ((s == "hexadecimal") || (s == "hexa") || (s == "hex"))
        {
            bb_log("<default mode hexadecimal>");
            numbers = t_num_hexadecimal;
        }
        else if ((s == "decimal") || (s == "dec"))
        {
            bb_log("<default mode decimal>");
            numbers = t_num_decimal;
        }
        else if ((s == "octal") || (s == "oct"))
        {
            bb_log("<default mode octal>");
            numbers = t_num_octal;
        }
        else if ((s == "binary") || (s == "bin"))
        {
            bb_log("<default mode binary>");
            numbers = t_num_binary;
        }

        // convert the string

        else
        {
            // string
            if (starts_with(s, "\"") || starts_with(s, "\'"))
            {
                bb_log("<string>");
                // remove delimiters
                s = s.substr(1, s.size() - 2);
                curr_type = t_string;
                add_to_bin_output(curr_type, endianess, s);
            }

            // number(s)
            else
            {
                std::istringstream ss(s);
                std::string w;
                while(ss >> w)
                {
                    // explicit hexa number
                    if (starts_with(w, "0x"))
                    {
                        bb_log("<explicit hexa>");
                        w = w.substr(2, w.size() - 2);
                        curr_type = t_num_hexadecimal;
                    }

                    // explicit decimal number
                    else if (starts_with(w, "0i"))
                    {
                        bb_log("<explicit decimal>");
                        w = w.substr(2, w.size() - 2);
                        curr_type = t_num_decimal;
                    }

                    // explicit octal number
                    else if (starts_with(w, "0o"))
                    {
                        bb_log("<explicit octal>");
                        w = w.substr(2, w.size() - 2);
                        curr_type = t_num_octal;
                    }

                    // explicit binary number
                    else if (starts_with(s, "0y"))
                    {
                        bb_log("<explicit binary>");
                        w = w.substr(2, w.size() - 2);
                        curr_type = t_num_binary;
                    }

                    // number with current type
                    else
                    {
                        bb_log("<number>");
                        curr_type = numbers;
                    }
                    add_to_bin_output(curr_type, endianess, w);
                }
            }
        }
    }
    std::stringstream ss;
    ss << "<generated " << m_output.size() << " bytes>";
    bb_log(ss.str());
}

/**
 * @brief Add an element to the binary output
 * If the element is a number, it will therefore use an output size depending on
 * its value or for hexa on the string size.
 *
 * @param stype the element type (raw string, hexa, decimal,...)
 * @param etype the endianess of the output
 * @param s the string representing the element
 *
 * @exception ExceptionBBInterpretation unable to interpret the element
 */
void BinMaker::add_to_bin_output(const type_t stype, const endianess_t etype,
        const std::string& s)
{
    const char *p;
    uint64_t val_u64;

    p = s.c_str();

    // string
    if (stype == t_string)
    {
        m_output.insert(m_output.end(), p, p + s.size());
    }

    // number
    else
    {
        int base;
        if (stype == t_num_hexadecimal)
        {
            base = 16;
        }
        else if (stype == t_num_decimal)
        {
            base = 10;
        }
        else if (stype == t_num_octal)
        {
            base = 8;
        }
        else if (stype == t_num_binary)
        {
            base = 2;
        }
        else
        {
            throw ExceptionBBInterpretation();
        }

        // convert from ascii to number
        val_u64 = (uint64_t)std::stoul(s, 0, base);
        p = (char*)&val_u64;

        // big-endian
        if (etype == big_endian)
        {
            if ((val_u64 > MAX_U32b_VALUE) ||
                    ((stype == t_num_hexadecimal) && (s.size() > 8)))
            {
                m_output.push_back(p[7]);
                m_output.push_back(p[6]);
                m_output.push_back(p[5]);
                m_output.push_back(p[4]);
            }
            if ((val_u64 > MAX_U16b_VALUE) ||
                    ((stype == t_num_hexadecimal) && (s.size() > 4)))
            {
                m_output.push_back(p[3]);
                m_output.push_back(p[2]);
            }
            if (val_u64 > MAX_U8b_VALUE)
            {
                m_output.push_back(p[1]);
                m_output.push_back(p[0]);
            }
            else
            {
                m_output.push_back(p[0]);
            }
        }
        // little-endian
        else
        {
            if (val_u64 > MAX_U8b_VALUE)
            {
                m_output.push_back(p[0]);
                m_output.push_back(p[1]);
            }
            else
            {
                m_output.push_back(p[0]);
            }
            if ((val_u64 > MAX_U16b_VALUE) ||
                    ((stype == t_num_hexadecimal) && (s.size() > 4)))
            {
                m_output.push_back(p[2]);
                m_output.push_back(p[3]);
            }
            if ((val_u64 > MAX_U32b_VALUE) ||
                    ((stype == t_num_hexadecimal) && (s.size() > 8)))
            {
                m_output.push_back(p[4]);
                m_output.push_back(p[5]);
                m_output.push_back(p[6]);
                m_output.push_back(p[7]);
            }
        }
    }
}

void BinMaker::bb_log(std::string msg)
{
    if (m_verbose)
    {
        std::clog << msg << std::endl;
    }
}
