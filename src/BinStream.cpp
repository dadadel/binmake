/*
 * BinStream.cpp
 *
 *  Created on: 2 janv. 2017
 *  Author: Adel Daouzli
 *  License: MIT License
 */

#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>

#include "utils.h"
#include "BinStream.h"

BinStream::BinStream()
		: m_input(), m_output(),
		  curr_endianess(little_endian),
		  curr_numbers(t_num_hexadecimal),
		  curr_type(t_none),
		  m_verbose(true)
{
}

BinStream::~BinStream()
{
}

/**
 * @brief Get the binary generated output.
 *
 * @param output will contain the output binary data
 *
 * @return true if the output was generated else false
 */
bool BinStream::get_binary(std::vector<char>& output)
{
    output = m_output;
    return true;
}

void BinStream::add_element(const std::string & element)
{
	std::stringstream sselem(element);
	std::string selem;
	std::string s;
	std::string tmp;
	size_t tmp_start, tmp_end;
	bool curr_string = false;
	char lim;

	while(getline(sselem, selem))
	{
        strip(selem);

        // ignore line

        if (starts_with(selem, "#") || selem.size() == 0)
        {
            bb_log("<ignore line>");
        }
        else if(starts_with(selem, "\"") || starts_with(selem, "\'"))
        {
        	proceed_element(selem);
        }
        else
        {
        	std::stringstream ss(selem);
			while(ss >> s)
			{
				proceed_element(s);
				/*if (starts_with(s, "\"") || starts_with(s, "\'"))
				{
					// case of a string
					//TODO: if (curr_string) should raise exception !
					lim = s[0];
					tmp = s;
					if (ss.tellg() != std::string::npos)
					{
						tmp_start = (size_t)ss.tellg() - (size_t)s.size();
					}
					else
					{
						ss.clear();
						ss.seekg(0, std::ios::end);
						tmp_start = (size_t)ss.tellg() - (size_t)s.size();
					}
					curr_string = true;
					if (endswith(s, std::string(1, lim)))
					{
						curr_string = false;
						tmp_end = (size_t)ss.tellg();
						tmp = element.substr(tmp_start, tmp_end-tmp_start);
						proceed_element(tmp);
					}
				}
				else
				{
					if (curr_string)
					{
						// case of a string currently parsed
						rstrip(s);
						if (endswith(s, std::string(1, lim)))
						{
							curr_string = false;
							if (ss.tellg() != std::string::npos)
							{
								tmp_end = (size_t)ss.tellg();
							}
							else
							{
								ss.clear();
								ss.seekg(0, std::ios::end);
								tmp_end = (size_t)ss.tellg();
							}
							tmp = element.substr(tmp_start, tmp_end-tmp_start);
							proceed_element(tmp);
						}
					}
					else
					{
						proceed_element(s);
					}
				}*/
			}
        }
	}
	//TODO: if (curr_string) should raise exception !
}

void BinStream::proceed_element(const std::string & element)
{
	std::string s(element);
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
        curr_endianess = little_endian;
    }
    else if(s == "big-endian")
    {
        bb_log("<default endianess big-endian>");
        curr_endianess = big_endian;
    }

    // change current number type

    else if ((s == "hexadecimal") || (s == "hexa") || (s == "hex"))
    {
        bb_log("<default mode hexadecimal>");
        curr_numbers = t_num_hexadecimal;
    }
    else if ((s == "decimal") || (s == "dec"))
    {
        bb_log("<default mode decimal>");
        curr_numbers = t_num_decimal;
    }
    else if ((s == "octal") || (s == "oct"))
    {
        bb_log("<default mode octal>");
        curr_numbers = t_num_octal;
    }
    else if ((s == "binary") || (s == "bin"))
    {
        bb_log("<default mode binary>");
        curr_numbers = t_num_binary;
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
            update_bin_output(curr_type, curr_endianess, s);
        }

        // number(s)
        else
        {
			// explicit hexa number
			if (starts_with(s, PREFIX_NUMBER_HEXADECIMAL))
			{
				bb_log("<explicit hexa>");
				s = s.substr(2, s.size() - 2);
				curr_type = t_num_hexadecimal;
			}

			// explicit decimal number
			else if (starts_with(s, PREFIX_NUMBER_DECIMAL))
			{
				bb_log("<explicit decimal>");
				s = s.substr(2, s.size() - 2);
				curr_type = t_num_decimal;
			}

			// explicit octal number
			else if (starts_with(s, PREFIX_NUMBER_OCTAL))
			{
				bb_log("<explicit octal>");
				s = s.substr(2, s.size() - 2);
				curr_type = t_num_octal;
			}

			// explicit binary number
			else if (starts_with(s, PREFIX_NUMBER_BINARY))
			{
				bb_log("<explicit binary>");
				s = s.substr(2, s.size() - 2);
				curr_type = t_num_binary;
			}

			// number with current type
			else
			{
				bb_log("<number>");
				curr_type = curr_numbers;
			}
			update_bin_output(curr_type, curr_endianess, s);
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
 * @exception ExceptionBBInterpretation unable to interpret the element
 */
void BinStream::update_bin_output(const type_t stype, const endianess_t etype,
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
        std::cout<<"s="<<s<<", base="<<base<<std::endl;
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

void BinStream::bb_log(std::string msg)
{
    if (m_verbose)
    {
        std::clog << msg << std::endl;
    }
}
