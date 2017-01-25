/*
 * bs_data.h
 *
 *  Created on: 25 janv. 2017
 *  Author: Adel Daouzli
 *  License: MIT License
 */

#ifndef BIN_DATA_H_
#define BIN_DATA_H_

#include <cstdint>

namespace BS
{
    typedef enum
    {
        t_num_hexadecimal,
        t_num_decimal,
        t_num_octal,
        t_num_binary,
        t_string,
        t_action,
        t_none,
        t_error
    } type_t;

    typedef enum
    {
        t_8bits,
        t_16bits,
        t_32bits,
        t_64bits,
        t_number_error
    } type_number_t;

    typedef enum
    {
        little_endian,
        big_endian
    } endianess_t;

    typedef struct
    {
        bool is_set;
        type_number_t type;
        endianess_t endianess;
        int size;
        bool num_signed;
        union
        {
            uint64_t value_u64;
            int64_t value_i64;
            char *value_p;
        };
    } number_t;
}

#endif