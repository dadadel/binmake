#include <string>
#include <regex>
#include "bs_data.h"
#include "utils.h"
#include "bin_tools.h"

/**
 * @brief Check that an element is conform to the grammar of type it is supposed
 * to be.
 *
 * @param element the element to check
 * @param elem_type the supposed type of the element
 * @return true if the element is valid else false
 */
bool BS::check_grammar(const std::string & element, type_t elem_type)
{
    bool ret(false);
    std::regex pattern;
    switch(elem_type)
    {
    case t_string:
        //TODO
        ret = true;
        break;
    case t_num_hexadecimal:
        pattern = R"((%x)?[\da-fA-F]+(\[\d+\])?)";
        ret = std::regex_match(element, pattern);
        break;
    case t_num_decimal:
        pattern = R"((%d)?[+-]?\d+(\[\d+\])?)";
        ret = std::regex_match(element, pattern);
        break;
    case t_num_octal:
        pattern = R"((%o)?[0-7]+(\[\d+\])?)";
        ret = std::regex_match(element, pattern);
        break;
    case t_num_binary:
        pattern = R"((%b)?[01]+(\[\d+\])?)";
        ret = std::regex_match(element, pattern);
        break;
    case t_internal_state:
        ret = is_internal_state(element);
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
BS::type_t BS::get_type(const std::string & element)
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

    else
    {
        // Element is an internal state
        if (is_internal_state(element))
        {
            ret = t_internal_state;
        }
    }

    return ret;
}

/**
 * @brief Add a number to a binary char vector after converting it
 *
 * @param v the char vector to store the converted number
 * @param number the number to convert and add (it contains the endianess
 * and the size)
 */
void BS::add_number_to_vector_char(std::vector<char> & v, const number_t number)
{
    char *p;
    p = (char*)((void*)&number.value_u64);
    if (number.endianess == big_endian)
    {
        for (int i = number.size-1; i >= 0; --i)
        {
            v.push_back(p[i]);
        }
    }
    else
    {
        for (int i = 0; i < number.size; ++i)
        {
            v.push_back(p[i]);
        }
    }
}

/**
 * @brief Check if an element is an internal state
 *
 * @param element the string element to check
 * @return true if is an internal state
 */
bool BS::is_internal_state(const std::string & element)
{
    state_type_t tmp_state;
    return get_state_type(element, tmp_state);
}

/**
 * @brief Get the type of state
 *
 * @param element the string element to process
 * @param state_type will contain the type
 * @return true if is success else false
 */
bool BS::get_state_type(const std::string & element, state_type_t & state_type)
{
    bool ret = true;
    std::string s(element);
    type_t tmp_type;
    endianess_t tmp_endian;
    int tmp_size = -1;

    strip(s);

    // check endianess

    if (extract_endianess(s, tmp_endian))
    {
        state_type = t_state_type_endianess;
    }

    // check number type
    else if (extract_number_type(s, tmp_type))
    {
        state_type = t_state_type_number;
    }

    // check size
    else if (extract_size(s, tmp_size))
    {
        state_type = t_state_type_size;
    }
    else if (tmp_size != -1)
    {
        state_type = t_state_type_size;
    }

    else
    {
        ret = false;
    }

    return ret;
}

/**
 * @brief Extract size provided in internal state 'size'
 * The size in bytes should be provided between brackets (e.g. "size[4]")
 *
 * @param str_size the string containing the state with the size
 * @param size will contain the extracted size
 * @return true if success else false (size can be set with if bad value)
 */

bool BS::extract_size(const std::string & str_size, int & size)
{
    bool ret(false);
    std::string s;
    std::regex pattern;
    std::smatch match;
    int value;

    pattern = R"(size\[(\d+)\])";
    if ( std::regex_search((str_size).begin(), (str_size).end(), match, pattern) )
    {
        s = match[1];
        value = std::stoi(s, 0, 10);
        if ((value == 0) || (value == 1) || (value == 2) ||
                (value == 4) || (value == 8))
        {
            size = value;
            ret = true;
        }
        else
        {
            // the bad value will be returned
            size = value;
            error_message("Bad size " + std::to_string(value) + " for default size. Should be 0, 1, 2, 4 or 8");
        }
    }
    return ret;
}

/**
 * @brief Extract endianess
 *
 * @param str_endian the string containing the endianess
 * @param endianess will contain the endianess
 * @true if success else false
 */
bool BS::extract_endianess(const std::string & str_endian, endianess_t & endianess)
{
    bool ret(true);
    if (str_endian == "little-endian")
    {
        endianess = little_endian;
    }
    else if (str_endian == "big-endian")
    {
        endianess = big_endian;
    }
    else
    {
        ret = false;
    }
    return ret;
}

/**
 * @brief Extract number type (hexadecimal, decimal,...)
 *
 * @param str_num the string containing the number type
 * @param num_type will contain the number type
 * @true if success else false
 */
bool BS::extract_number_type(const std::string & str_num, type_t & num_type)
{
    bool ret(true);
    if ((str_num == "hexadecimal") || (str_num == "hexa") || (str_num == "hex"))
    {
        num_type = t_num_hexadecimal;
    }
    else if ((str_num == "decimal") || (str_num == "dec"))
    {
        num_type = t_num_decimal;
    }
    else if ((str_num == "octal") || (str_num == "oct"))
    {
        num_type = t_num_octal;
    }
    else if ((str_num == "binary") || (str_num == "bin"))
    {
        num_type = t_num_binary;
    }
    else
    {
        ret = false;
    }
    return ret;
}


/**
 * @brief Build a number represented in a string.
 *
 * @param element the string representing the number
 * @param number will store the extracted number
 * @param elem_type the supposed type of the element (should be a number type)
 * @param size the size in bytes of the target element. If 0 will determine
 * a size based on its value if decimal or octal or the number of characters
 * if hexadecimal or binary (default 0)
 * size will be ignored if a size is explicitly provided (e.g. "12[4]" -> size 4)
 * @return true if the number was extracted else false
 */
bool BS::build_number(const std::string & element, number_t  & number,
        const type_t elem_type, const endianess_t endian, const int elem_size)
{
    bool ret(true);
    int base;
    bool num_signed;
    std::string s;
    std::regex pattern;
    std::smatch match;
    int64_t val_i64;
    uint64_t val_u64;
    unsigned int nb_char = 0;
    int size = elem_size;

    number.is_set = false;
    // check size
    if ((size != 0) && (size != 1) && (size != 2) && (size != 4) && (size != 8))
    {
        ret = false;
        error_message("Invalid target size " + std::to_string(size) + " for number to extract");
    }
    // check number grammar
    if (!check_grammar(element, elem_type))
    {
        ret = false;
        error_message("Invalid element type " + std::to_string(elem_type) + " for number to extract");
    }
    // check base
    if (ret)
    {
        switch(elem_type)
        {
        case t_num_hexadecimal:
            base = 16;
            nb_char = 2;
            break;
        case t_num_decimal:
            base = 10;
            break;
        case t_num_octal:
            base = 8;
            break;
        case t_num_binary:
            base = 2;
            nb_char = 8;
            break;
        default:
            error_message("Bad element type " + std::to_string(elem_type) + " for a number");
            ret = false;
            break;
        }
    }
    // extract the number part substring and eventually the size if explicit
    pattern = R"((%\w{1})?([+-]?[\da-fA-F]+)(\[\d+\])?)";
    if (!std::regex_search(element.begin(), element.end(), match, pattern))
    {
        error_message("Failed to parse element number '" + element + "'");
        ret = false;
    }
    else
    {
        // get size if provided
        s = match[3];
        if (!s.empty())
        {
            s = s.substr(1, s.size() - 2);
            size = std::stoi(s, 0, 10);
        }
        // get substring representing the number (with possible sign)
        s = match[2];
    }
    // convert ASCII to number
    if (ret)
    {
        if (starts_with(s, "-"))
        {
            num_signed = true;
            val_i64 = (int64_t)std::stoll(s, 0, base);
        }
        else
        {
            num_signed = false;
            val_u64 = (uint64_t)std::stoull(s, 0, base);
        }
    }
    // determine size if still not done
    if (ret && (size == 0))
    {
        switch(elem_type)
        {
        // hexa and binary depend on number of characters
        case t_num_hexadecimal:
        case t_num_binary:
            if (s.size() > (4 * nb_char))
            {
                size = 8;
            }
            else if (s.size() > (2 * nb_char))
            {
                size = 4;
            }
            else if (s.size() > (1 * nb_char))
            {
                size = 2;
            }
            else
            {
                size = 1;
            }
            break;
        // decimal and octal depend on the value
        case t_num_decimal:
        case t_num_octal:
            int n;
            int b;
            if (num_signed && (elem_type == t_num_decimal))
            {
                int64_t bits;
                for (int i=2; i >= 0; i--)
                {
                    b = pow(2, i);
                    n = b * 8;
                    bits = pow(2, n-1);
                    // min (negative) and max values in bits
                    if ((val_i64 > (bits - 1)) || (val_i64 < (-bits)))
                    {
                        size = b * 2;
                        break;
                    }
                }
            }
            else if (!num_signed)
            {
                for (int i=2; i >= 0; i--)
                {
                    uint64_t ubits;
                    b = pow(2, i);
                    n = b * 8;
                    ubits = pow(2, n);
                    if (val_u64 > (ubits - 1))
                    {
                        size = b * 2;
                        break;
                    }
                }
            }
            else
            {
                ret = false;
                //TODO bs_log("Unexpected error (signed = %d, elem type = %d)", (int)num_signed, (int)elem_type);
                error_message("Unexpected error (signed = " + std::to_string(num_signed)+ ", elem type = " + std::to_string(elem_type));
            }
            if (size == 0)
            {
                size = 1;
            }
            break;
        default:
            error_message("Unexpected element type. This should never happen !");
            ret = false;
            break;
        }
    }
    // set the number
    if (ret)
    {
        number.is_set = true;
        number.endianess = endian;
        if (num_signed)
        {
            number.value_i64 = val_i64;
            number.num_signed = true;
        }
        else
        {
            number.value_u64 = val_u64;
            number.num_signed = false;
        }
        if ((size != 1) && (size != 2) && (size != 4) && (size != 8))
        {
            ret = false;
            number.is_set = false;
            error_message("Unexpected size !");
        }
        else
        {
            number.size = size;
        }
    }
    return ret;
}

/**
 * @brief Extract to a string a number from binary data. The string is built
 * following a representation provided in a string description
 */
void BS::extract_number(std::string & str_number, const std::vector<char> element,
        const std::string & description, const endianess_t & endianess, const int size)
{
    //TODO
}
