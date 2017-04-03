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

// WARNING: this can be wrong. Supposing float is 32 bit and double 64 bits.
typedef float float32_t;
typedef double float64_t;

namespace BS
{
    typedef enum
    {
        t_state_type_string,
        t_state_type_number,
        t_state_type_size,
        t_state_type_endianess,
        t_state_type_error
    } state_type_t;

    typedef enum
    {
        t_num_hexadecimal,
        t_num_decimal,
        t_num_octal,
        t_num_binary,
        t_num_float,
        t_string,
        t_internal_state,
        t_none,
        t_error
    } type_t;

    typedef enum
    {
        little_endian,
        big_endian
    } endianess_t;

    typedef struct
    {
        bool is_set;
        endianess_t endianess;
        int size; /** size in bytes (0, 1, 2, 4 or 8) */
        bool num_signed;
        union
        {
            uint64_t value_u64;
            int64_t value_i64;
            float32_t value_f32;
            float64_t value_f64;
        };
    } number_t;
}

#endif
