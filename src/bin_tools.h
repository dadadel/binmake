/*
 * bin_tools.h
 *
 *  Created on: 25 janv. 2017
 *  Author: Adel Daouzli
 *  License: MIT License
 */

#ifndef BIN_TOOLS_H_
#define BIN_TOOLS_H_

#include <string>
#include <vector>

#include "bs_data.h"

namespace BS
{
bool is_internal_state(const std::string & element);
type_t get_type(const std::string & element);
bool get_state_type(const std::string & element, state_type_t & state_type);
bool check_grammar(const std::string & element, type_t elem_type);
void add_number_to_vector_char(std::vector<char> & v, const number_t number);
bool build_number(const std::string & element, number_t & number,
        const type_t elem_type, const endianess_t endian, const int size=0);
void extract_number(std::string & str_number, const std::vector<char> element,
        const std::string & description, const endianess_t & endianess,
        const int size);
bool extract_size(const std::string & str_size, int & size);
bool extract_endianess(const std::string & str_endian, endianess_t & endianess);
bool extract_number_type(const std::string & str_num, type_t & num_type);
}

#endif