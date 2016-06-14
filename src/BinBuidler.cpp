/*
 * BinBuidler.cpp
 *
 *  Created on: 14 juin 2016
 *      Author: adaouzli
 */

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

#include "BinBuidler.h"

/**
 * @brief Default constructor
 */
BinBuidler::BinBuidler()
    : m_output(), m_input_available(false),
      m_output_generated(false), m_output_updated(false)
{
}

/**
 * @brief Copy constructor
 */
BinBuidler::BinBuidler(const BinBuidler &b)
    : m_output(), m_input_available(b.m_input_available),
      m_output_generated(false), m_output_updated(false)
{
    m_input.str(b.m_input.str());
}

/**
 * @brief String copy constructor
 *
 * @param s the string to set as input text
 */
BinBuidler::BinBuidler(std::string& s)
: m_output(), m_input_available(false),
  m_output_generated(false), m_output_updated(false)
{
    m_input.str() = s;
}

/**
 * @brief String stream copy constructor
 *
 * @param s the string stream to set as input text
 */
BinBuidler::BinBuidler(std::stringstream& s)
: m_output(), m_input_available(false),
  m_output_generated(false), m_output_updated(false)
{
    m_input.str(s.str());
}

/**
 * @brief Destructor
 */
BinBuidler::~BinBuidler()
{
}

/**
 * @brief Build the output binary
 *
 * @return true if built the binary else false
 */
bool BinBuidler::buildbin(void)
{
    bool ret = false;
    if (m_input_available)
    {
        m_output.clear();
        parse_data();
        ret = true;
    }
    return ret;
}

void BinBuidler::set_input(const std::stringstream& input)
{
    m_input.str(input.str());
    m_input_available = true;
}

void BinBuidler::set_input(const std::string& input)
{
    m_input.str(input);
    m_input_available = true;
}

bool BinBuidler::buildbin(const std::stringstream& input)
{
}

bool BinBuidler::buildbin(const std::string& input)
{
}

void BinBuidler::parse_data(void)
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
            std::cout << "<ignore line>"<<std::endl;
        }

        // change current endianness

        else if(s == "little-endian")
        {
            std::cout<<"<le mode>"<<std::endl;
            endianess = little_endian;
        }
        else if(s == "big-endian")
        {
            std::cout<<"<be mode>"<<std::endl;
            endianess = big_endian;
        }

        // change current number type

        else if ((s == "hexadecimal") || (s == "hexa") || (s == "hex"))
        {
            numbers = t_num_hexadecimal;
        }
        else if ((s == "decimal") || (s == "dec"))
        {
            numbers = t_num_decimal;
        }
        else if ((s == "octal") || (s == "oct"))
        {
            numbers = t_num_octal;
        }
        else if ((s == "boolean") || (s == "bool"))
        {
            numbers = t_num_boolean;
        }

        // convert the string

        else
        {
            // string
            if (starts_with(s, "\"") || starts_with(s, "\'"))
            {
                std::cout<<"<string>";
                // remove delimiters
                s = s.substr(1, s.size() - 2);
                curr_type = t_string;
            }

            // explicit hexa number
            else if (starts_with(s, "0x"))
            {
                s = s.substr(2, s.size() - 2);
                curr_type = t_num_hexadecimal;
            }

            // explicit decimal number
            else if (starts_with(s, "0d"))
            {
                s = s.substr(2, s.size() - 2);
                curr_type = t_num_decimal;
            }

            // explicit octal number
            else if (starts_with(s, "0o"))
            {
                s = s.substr(2, s.size() - 2);
                curr_type = t_num_octal;
            }

            // explicit boolean number
            else if (starts_with(s, "0b"))
            {
                s = s.substr(2, s.size() - 2);
                curr_type = t_num_boolean;
            }

            // number with current type
            else
            {
                curr_type = numbers;
            }

            add_to_bin_output(curr_type, endianess, s);
        }
    }
    //TODO remove
    for (int i=0;i<m_output.size();i++)
    {
        std::cout << std::hex << (unsigned int)m_output[i] << " ";
    }
}

void BinBuidler::add_to_bin_output(const type_t stype, const endianess_t etype,
        const std::string& s)
{
    const char *p;
    std::stringstream ss;
    uint32_t val_u32;
    uint16_t val_u16;

    p = s.c_str();

    // string
    if (stype == t_string)
    {
        m_output.insert(m_output.end(), p, p + s.size());
    }

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
        //TODO update size checking for decimal
        if (s.size() == 4)
        {
            val_u16 = (uint16_t)std::stoul(s, 0, base);
            p = (char*)&val_u16;
            if (etype == big_endian)
            {
                m_output.push_back(p[1]);
                m_output.push_back(p[0]);
            }
            else
            {
                m_output.push_back(p[0]);
                m_output.push_back(p[1]);
            }
        }
        else if (s.size() == 8)
        {
            val_u32 = (uint32_t)std::stoul(s, 0, base);
            p = (char*)&val_u32;
            if (etype == big_endian)
            {
                m_output.push_back(p[3]);
                m_output.push_back(p[2]);
                m_output.push_back(p[1]);
                m_output.push_back(p[0]);
            }
            else
            {
                m_output.push_back(p[0]);
                m_output.push_back(p[1]);
                m_output.push_back(p[2]);
                m_output.push_back(p[3]);
            }
        }
    }
}

bool BinBuidler::get_binary(std::vector<char>& output) const
{
    bool ret = false;
    output = m_output;
    return ret;
}
