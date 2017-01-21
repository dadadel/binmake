/*
 * BinStream.cpp
 *
 *  Created on: 2 janv. 2017
 *  Author: Adel Daouzli
 *  License: MIT License
 */

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>

#include "utils.h"
#include "BinStream.h"

using namespace BS;

BS::BinStream::BinStream(bool verbose)
        : m_input(), m_output(),
          m_curr_endianess(little_endian),
          m_curr_numbers(t_num_hexadecimal),
          m_input_ready(false),
          m_output_ready(false),
          m_verbose(verbose)
{
}

BS::BinStream::BinStream(const BS::BinStream& o)
        : m_curr_endianess(o.m_curr_endianess),
          m_curr_numbers(o.m_curr_numbers),
          m_input_ready(o.m_input_ready),
          m_output_ready(o.m_output_ready),
          m_verbose(o.m_verbose)
{
    if (o.m_output_ready)
    {
        m_output = o.m_output;
    }
    if (o.m_input_ready)
    {
        m_input.str(o.m_input.str());
    }
}

BS::BinStream::~BinStream()
{
}

/**
 * @brief Reset the input, the output and the parsing modes.
 */
void BS::BinStream::reset(void)
{
    reset_modes();
    reset_output();
    reset_input();
}

/**
 * @brief Reset to default the modes of parsing, that is hexadecimal
 * for numbers and little-endian for endianess
 */
void BS::BinStream::reset_modes(void)
{
    m_curr_endianess = little_endian;
    m_curr_numbers = t_num_hexadecimal;
}

/**
 * @brief Reset the input data.
 */
void BS::BinStream::reset_output(void)
{
    m_output_ready = false;
    m_output.clear();
}

/**
 * @brief Reset the output data.
 */
void BS::BinStream::reset_input(void)
{
    m_input_ready = false;
    m_input.str("");
    m_input.clear();
}

/**
 * @brief Check if an input is available.
 *
 * @return true if ready else false
 */
bool BS::BinStream::input_ready(void) const
{
    return m_input_ready;
}

/**
 * @brief Check if an output is generated.
 *
 * @return true if ready else false
 */
bool BS::BinStream::output_ready(void) const
{
    return m_output_ready;
}

/**
 * @brief Get the binary generated output.
 *
 * @param output will contain the output binary data
 * @return true if got the output else false (not output generated)
 */
bool BS::BinStream::get_output(std::vector<char>& output) const
{
    if (m_output_ready)
    {
        output = m_output;
    }
    return m_output_ready;
}

size_t BS::BinStream::size(void) const
{
    if (m_output_ready)
    {
        return m_output.size();
    }
    return 0;
}

char BS::BinStream::operator[](const size_t index) const
{
    if (!m_output_ready)
    {
        throw BSExceptionNoOutputAvailable();
    }
    if (index >= m_output.size())
    {
        throw BSExceptionOutOfRange(index);
    }
    return m_output.data()[index];
}

//////////////////////////    STREAM OPERATORS    //////////////////////////////

/**
 * @brief Add and parse an input stream.
 * The output will be updated.
 *
 * @param f the istream input file
 * @return the instance
 */
BS::BinStream& BS::BinStream::operator<<(const std::istream & s)
{
    std::stringstream ss;
    ss << s.rdbuf();
    parse_input(ss.str());
    return *this;
}

/**
 * @brief Add and parse an input stream from file.
 * The output will be updated.
 *
 * @param f the ifstream input file
 * @return the instance
 */
BS::BinStream& BS::BinStream::operator<<(const std::ifstream & f)
{
    std::stringstream ss;

    if (f.is_open())
    {
        ss << f.rdbuf();
        parse_input(ss.str());
    }
    return *this;
}

/**
 * @brief Add and parse an input stream from a string stream.
 * The output will be updated.
 *
 * @param desc the stringstream input
 * @return the instance
 */
BS::BinStream& BS::BinStream::operator<<(const std::stringstream & desc)
{
    parse_input(desc.str());
    return *this;
}

/**
 * @brief Add and parse an input stream from a string.
 * The output will be updated.
 *
 * @param desc the string input
 * @return the instance
 */
BS::BinStream& BS::BinStream::operator<<(const std::string & desc)
{
    parse_input(desc);
    return *this;
}

/**
 * @brief Get the binary output if available.
 * If the output was not generated will raise an exception
 *
 * @param output the char vector to store the output
 * @return the instance
 * @exception BSExceptionNoOutputAvailable the output was not generated
 */
BS::BinStream& BS::BinStream::operator>>(std::vector<char> & output)
{
    if (m_output_ready)
    {
        get_output(output);
    }
    else
    {
        throw BSExceptionNoOutputAvailable();
    }
    return *this;
}

/**
 * @brief Save the output to a file
 *
 * @param f the ofstream file
 * @return the instance
 * @exception BSExceptionNoOutputAvailable the output was not generated
 */
BS::BinStream& BS::BinStream::operator>>(std::ofstream & f)
{
    if (m_output_ready)
    {
        f.write(m_output.data(), m_output.size());
    }
    else
    {
        throw BSExceptionNoOutputAvailable();
    }
    return *this;
}

namespace BS {
/**
 * @brief Stream the output to a friend ostream
 *
 * @param stream the ostream
 * @param bin_stream the BinStream instance containing the output
 * @return the ostream
 */
std::ostream& operator<<(std::ostream& stream, const BinStream& bin_stream)
{
    for (size_t i = 0; i < bin_stream.size(); ++i)
    {
        stream << bin_stream[i];
    }
    return stream;
}

/**
 * @brief Add a friend istream input and parse it.
 * The output will be updated.
 *
 * @param stream the istream input
 * @param bin_stream the BinStream instance to update
 * @return the istream
 */
std::istream& operator>>(std::istream& stream, BinStream& bin_stream)
{
    bin_stream << stream;
    return stream;
}
}



///////////////////////    LOW-LEVEL FUNCTIONS    //////////////////////////////


/**
 * @brief Parse an input and update the output.
 *
 *
 * @param element the string containing the input data to proceed
 * @return the instance
 */
void BS::BinStream::parse_input(const std::string & element)
{
    std::stringstream sselem(element);
    std::string line;

    m_input << element;
    m_input_ready = true;

    while(getline(sselem, line))
    {
        strip(line);

        // comment so ignore the line
        if (starts_with(line, "#") || line.size() == 0)
        {
            bs_log("<ignore line>");
        }
        // line is a string
        else if(starts_with(line, "\"") || starts_with(line, "\'"))
        {
            proceed_element(line);
        }
        // other: parse the line word after word
        else
        {
            std::stringstream ss(line);
            std::string word;
            while(ss >> word)
            {
                proceed_element(word);
            }
        }
    }
}

bool BS::BinStream::check_grammar(const std::string & element, type_t elem_type)
{
    bool ret(false);
    std::regex pattern;
    switch(elem_type)
    {
    case t_string:
        //TODO
        break;
    case t_num_hexadecimal:
        pattern = R"([\da-fA-F]+)";
        ret = std::regex_match(element, pattern);
        break;
    case t_num_decimal:
        pattern = R"([+-]?\d+)";
        ret = std::regex_match(element, pattern);
        break;
    case t_num_octal:
        pattern = R"([+-]?[0-7]+)";
        ret = std::regex_match(element, pattern);
        break;
    case t_num_binary:
        pattern = R"([01]+)";
        ret = std::regex_match(element, pattern);
        break;
    case t_none:
        //TODO
        break;
    case t_error:
        //TODO
        break;
    }
    return ret;
}

/**
 * @brief Get the type of an element
 *
 * @param element the element to determine type
 * @return the detected type
 */
type_t BS::BinStream::get_type(const std::string & element)
{
    type_t ret = t_none;
    bool res_comp;

    std::regex pattern;

    // Element is an explicit number

    if (starts_with(element, PREFIX_NUMBER))
    {
        pattern = R"(%[dxbo]{1}\S+)";
        res_comp = std::regex_match(element, pattern);
        if (!res_comp)
        {
            ret = t_error;
        }
        else
        {
            if (starts_with(element, PREFIX_NUMBER_HEXADECIMAL))
            {
                ret = t_num_hexadecimal;
            }
            else if (starts_with(element, PREFIX_NUMBER_DECIMAL))
            {
                ret = t_num_decimal;
            }
            else if (starts_with(element, PREFIX_NUMBER_OCTAL))
            {
                ret = t_num_octal;
            }
            else if (starts_with(element, PREFIX_NUMBER_BINARY))
            {
                ret = t_num_binary;
            }
            else
            {
                ret = t_error;
            }
        }
    }

    // Element is a string

    else if ((starts_with(element, "\"") || starts_with(element, "\'")))
    {
        ret = t_string;
    }

    return ret;
}

void BS::BinStream::extract_number(number_t number, const std::string & element,
        const type_number_t & element_type, const endianess_t & endianess, const int size)
{
    uint64_t val_u64;
    int base;
    endianess_t etype;
    type_t stype;
    char *p;
    //TODO
    ;
    // convert from ascii to number
    val_u64 = (uint64_t)std::stoul(element, 0, base);
    p = (char*)&val_u64;

    // big-endian
    if (etype == big_endian)
    {
        if ((size == 8) || (val_u64 > MAX_U32b_VALUE) ||
                ((stype == t_num_hexadecimal) && (element.size() > 8)))
        {
            m_output.push_back(p[7]);
            m_output.push_back(p[6]);
            m_output.push_back(p[5]);
            m_output.push_back(p[4]);
        }
        if ((size >= 4) || (val_u64 > MAX_U16b_VALUE) ||
                ((stype == t_num_hexadecimal) && (element.size() > 4)))
        {
            m_output.push_back(p[3]);
            m_output.push_back(p[2]);
        }
        if ((size >= 2) || (val_u64 > MAX_U8b_VALUE))
        {
            m_output.push_back(p[1]);
            m_output.push_back(p[0]);
        }
        else
        {
            m_output.push_back(p[0]);
        }
        m_output_ready = true;
    }
    // little-endian
    else
    {
        if ((size >= 2) || (val_u64 > MAX_U8b_VALUE))
        {
            m_output.push_back(p[0]);
            m_output.push_back(p[1]);
        }
        else
        {
            m_output.push_back(p[0]);
        }
        if ((size >= 4) || (val_u64 > MAX_U16b_VALUE) ||
                ((stype == t_num_hexadecimal) && (element.size() > 4)))
        {
            m_output.push_back(p[2]);
            m_output.push_back(p[3]);
        }
        if ((size == 8) || (val_u64 > MAX_U32b_VALUE) ||
                ((stype == t_num_hexadecimal) && (element.size() > 8)))
        {
            m_output.push_back(p[4]);
            m_output.push_back(p[5]);
            m_output.push_back(p[6]);
            m_output.push_back(p[7]);
        }
        m_output_ready = true;
    }
}


/**
 * @brief Proceed an element and update the output.
 *
 *
 * @param element the string containing the input data to proceed
 * @return the instance
 */
void BS::BinStream::proceed_element(const std::string & element)
{
    std::string s(element);
    type_t curr_type;

    strip(s);

    // change current endianess

    if(s == "little-endian")
    {
        bs_log("<default endianess little-endian>");
        m_curr_endianess = little_endian;
    }
    else if(s == "big-endian")
    {
        bs_log("<default endianess big-endian>");
        m_curr_endianess = big_endian;
    }

    // change current number type

    else if ((s == "hexadecimal") || (s == "hexa") || (s == "hex"))
    {
        bs_log("<default mode hexadecimal>");
        m_curr_numbers = t_num_hexadecimal;
    }
    else if ((s == "decimal") || (s == "dec"))
    {
        bs_log("<default mode decimal>");
        m_curr_numbers = t_num_decimal;
    }
    else if ((s == "octal") || (s == "oct"))
    {
        bs_log("<default mode octal>");
        m_curr_numbers = t_num_octal;
    }
    else if ((s == "binary") || (s == "bin"))
    {
        bs_log("<default mode binary>");
        m_curr_numbers = t_num_binary;
    }

    // convert the input into binary output

    else
    {
        // string
        if (starts_with(s, "\"") || starts_with(s, "\'"))
        {
            bs_log("<string>");
            // remove delimiters
            s = s.substr(1, s.size() - 2);
            curr_type = t_string;
            update_bin_output(curr_type, m_curr_endianess, s);
        }

        // number(s)
        else
        {
            // explicit hexa number
            if (starts_with(s, PREFIX_NUMBER_HEXADECIMAL))
            {
                bs_log("<explicit hexa>");
                s = s.substr(2, s.size() - 2);
                curr_type = t_num_hexadecimal;
            }

            // explicit decimal number
            else if (starts_with(s, PREFIX_NUMBER_DECIMAL))
            {
                bs_log("<explicit decimal>");
                s = s.substr(2, s.size() - 2);
                curr_type = t_num_decimal;
            }

            // explicit octal number
            else if (starts_with(s, PREFIX_NUMBER_OCTAL))
            {
                bs_log("<explicit octal>");
                s = s.substr(2, s.size() - 2);
                curr_type = t_num_octal;
            }

            // explicit binary number
            else if (starts_with(s, PREFIX_NUMBER_BINARY))
            {
                bs_log("<explicit binary>");
                s = s.substr(2, s.size() - 2);
                curr_type = t_num_binary;
            }

            // number with current type
            else
            {
                bs_log("<number>");
                curr_type = m_curr_numbers;
            }
            update_bin_output(curr_type, m_curr_endianess, s);
        }
    }
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
 * @exception BSExceptionBadType unable to interpret the element
 */
void BS::BinStream::update_bin_output(const type_t stype,
        const endianess_t etype, const std::string& s)
{
    const char *p;
    size_t pos;
    size_t pos2;
    uint64_t val_u64;
    std::string s2;
    int size = 1;

    p = s.c_str();

    // string
    if (stype == t_string)
    {
        m_output.insert(m_output.end(), p, p + s.size());
        m_output_ready = true;
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
            throw BSExceptionBadType(stype);
        }

        // check if size is provided
        pos = s.find('[');
        if (pos != std::string::npos)
        {
            pos2 = s.find(']');
            if (pos2 == std::string::npos)
            {
                pos2 = s.size();
            }
            s2 = s.substr(pos + 1, pos2 - 1);
            size = std::stoul(s, 0, 10);
        }
        std::stringstream tmps;
        tmps << stype << ": check grammar:" << std::boolalpha << check_grammar(s, stype);
        bs_log(tmps.str());
        // convert from ascii to number
        val_u64 = (uint64_t)std::stoul(s, 0, base);
        p = (char*)&val_u64;

        // big-endian
        if (etype == big_endian)
        {
            if ((size == 8) || (val_u64 > MAX_U32b_VALUE) ||
                    ((stype == t_num_hexadecimal) && (s.size() > 8)))
            {
                m_output.push_back(p[7]);
                m_output.push_back(p[6]);
                m_output.push_back(p[5]);
                m_output.push_back(p[4]);
            }
            if ((size >= 4) || (val_u64 > MAX_U16b_VALUE) ||
                    ((stype == t_num_hexadecimal) && (s.size() > 4)))
            {
                m_output.push_back(p[3]);
                m_output.push_back(p[2]);
            }
            if ((size >= 2) || (val_u64 > MAX_U8b_VALUE))
            {
                m_output.push_back(p[1]);
                m_output.push_back(p[0]);
            }
            else
            {
                m_output.push_back(p[0]);
            }
            m_output_ready = true;
        }
        // little-endian
        else
        {
            if ((size >= 2) || (val_u64 > MAX_U8b_VALUE))
            {
                m_output.push_back(p[0]);
                m_output.push_back(p[1]);
            }
            else
            {
                m_output.push_back(p[0]);
            }
            if ((size >= 4) || (val_u64 > MAX_U16b_VALUE) ||
                    ((stype == t_num_hexadecimal) && (s.size() > 4)))
            {
                m_output.push_back(p[2]);
                m_output.push_back(p[3]);
            }
            if ((size == 8) || (val_u64 > MAX_U32b_VALUE) ||
                    ((stype == t_num_hexadecimal) && (s.size() > 8)))
            {
                m_output.push_back(p[4]);
                m_output.push_back(p[5]);
                m_output.push_back(p[6]);
                m_output.push_back(p[7]);
            }
            m_output_ready = true;
        }
    }
}

void BS::BinStream::set_verbosity(bool verbose)
{
    m_verbose = verbose;
}

void BS::BinStream::bs_log(std::string msg)
{
    if (m_verbose)
    {
        std::clog << msg << std::endl;
    }
}

