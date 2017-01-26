#include <string>
#include <vector>

#include "catch.hpp"
#include "bs_data.h"
#include "bin_tools.h"

using namespace std;
using namespace BS;

TEST_CASE("Test of bin_tools functions")
{
    SECTION("Unit test of 'get_type()'")
    {
        // Strings

        REQUIRE( get_type("'blabla'") == t_string );
        REQUIRE( get_type("\"blabla\"") == t_string );
        REQUIRE( get_type("'some text with spaces !'") == t_string );
        REQUIRE( get_type("'and text with\ttabs and\nreturn to line'") == t_string );

        // Cannot determine type so None

        REQUIRE( get_type("some text inside 1 string") == t_none );
        REQUIRE( get_type("-23") == t_none );
        REQUIRE( get_type("0a23") == t_none );
        REQUIRE( get_type("023") == t_none );
        REQUIRE( get_type("1101") == t_none );

        // Numbers

        REQUIRE( get_type("%x023") == t_num_hexadecimal );
        REQUIRE( get_type("%xffff") == t_num_hexadecimal );
        REQUIRE( get_type("%d-023") == t_num_decimal );
        REQUIRE( get_type("%o023") == t_num_octal );
        REQUIRE( get_type("%b01101") == t_num_binary );

        // Not valid number

        REQUIRE( get_type("%0101") == t_error );
        REQUIRE( get_type("%d-023") == t_num_decimal );

        // Actions

        REQUIRE( get_type("big-endian") == t_action );
        REQUIRE( get_type("size[5]") == t_action );
        REQUIRE( get_type("hexa") == t_action );
        REQUIRE( get_type("binary") == t_action );
    }

    SECTION("Unit test of 'is_action()'")
    {
        // Endianess

        REQUIRE( is_action("big-endian") );
        REQUIRE( is_action("little-endian") );

        // Size

        REQUIRE( is_action("size[5]") );

        // Numbers base

        REQUIRE( is_action("hexadecimal") );
        REQUIRE( is_action("hexa") );
        REQUIRE( is_action("hex") );
        REQUIRE( is_action("decimal") );
        REQUIRE( is_action("octal") );
        REQUIRE( is_action("oct") );
        REQUIRE( is_action("binary") );
        REQUIRE( is_action("bin") );

        // Not valid

        REQUIRE( is_action("blabla") == false );
    }

    SECTION("Unit test of 'check_grammar()'")
    {
        /////////  Explicit numbers  /////////

        // Valid grammar
        REQUIRE( check_grammar("%d-023", t_num_decimal) );
        REQUIRE( check_grammar("%d42", t_num_decimal) );
        REQUIRE( check_grammar("%d+42", t_num_decimal) );
        REQUIRE( check_grammar("%x023", t_num_hexadecimal) );
        REQUIRE( check_grammar("%x0a3", t_num_hexadecimal) );
        REQUIRE( check_grammar("%o023", t_num_octal) );
        REQUIRE( check_grammar("%b01101", t_num_binary) );

        // Not valid grammar
        REQUIRE( check_grammar("%x0g23", t_num_hexadecimal) == false );
        REQUIRE( check_grammar("%d0a3", t_num_decimal) == false );
        REQUIRE( check_grammar("%o09", t_num_octal) == false );
        REQUIRE( check_grammar("%b01102", t_num_binary) == false );

        //////// Not explicit numbers /////////

        // Valid grammar
        REQUIRE( check_grammar("-023", t_num_decimal) );
        REQUIRE( check_grammar("42", t_num_decimal) );
        REQUIRE( check_grammar("+42", t_num_decimal) );
        REQUIRE( check_grammar("023", t_num_hexadecimal) );
        REQUIRE( check_grammar("0a3", t_num_hexadecimal) );
        REQUIRE( check_grammar("023", t_num_octal) );
        REQUIRE( check_grammar("01101", t_num_binary) );

        // Not valid grammar
        REQUIRE( check_grammar("g23", t_num_hexadecimal) == false );
        REQUIRE( check_grammar("a3", t_num_decimal) == false );
        REQUIRE( check_grammar("09", t_num_octal) == false );
        REQUIRE( check_grammar("01102", t_num_binary) == false );


        // Grammar with size
        REQUIRE( check_grammar("42[4]", t_num_decimal) );
        REQUIRE( check_grammar("%d+42[4]", t_num_decimal) );
        // note: the size 7 is not allowed but the grammar is correct
        REQUIRE( check_grammar("%x023[7]", t_num_hexadecimal) );
        REQUIRE( check_grammar("%o47[1]", t_num_octal) );
        REQUIRE( check_grammar("%o947[1]", t_num_octal) == false );
        REQUIRE( check_grammar("%b01101[8]", t_num_binary) );
    }


    SECTION("Unit test of 'build_number()'")
    {
        string s;
        number_t num;

        s = "-42";
        REQUIRE( build_number(s, num, t_num_decimal, big_endian, 4) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 4 );
        REQUIRE( num.type == t_32bits );
        REQUIRE( num.num_signed );
        REQUIRE( num.value_i64 == -42 );

        s = "42";
        REQUIRE( build_number(s, num, t_num_decimal, big_endian) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 1 );
        REQUIRE( num.type == t_8bits );
        REQUIRE( !num.num_signed );
        REQUIRE( num.value_u64 == 42 );

        s = "%xa42";
        REQUIRE( build_number(s, num, t_num_hexadecimal, big_endian) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 2 );
        REQUIRE( num.type == t_16bits );
        REQUIRE( !num.num_signed );
        REQUIRE( num.value_u64 == 0x0a42 );

        REQUIRE( build_number(s, num, t_num_hexadecimal, little_endian) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == little_endian );
        REQUIRE( num.value_u64 == 0x0a42 );

        s = "000a42";
        REQUIRE( build_number(s, num, t_num_hexadecimal, big_endian) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 4 );
        REQUIRE( num.type == t_32bits );
        REQUIRE( !num.num_signed );
        REQUIRE( num.value_u64 == 0x0a42 );
    }

    SECTION("Unit test of 'add_number_to_vector_char()'")
    {
        vector<char> v;
        number_t number;
        number.is_set = true;
        number.type = t_32bits;
        number.size = 4;
        number.num_signed = false;
        number.value_u64 = 0x00112233;

        // big-endian

        number.endianess = big_endian;

        add_number_to_vector_char(v, number);

        REQUIRE( v.size() == 4 );
        REQUIRE( v.data()[0] == 0x00 );
        REQUIRE( v.data()[1] == 0x11 );
        REQUIRE( v.data()[2] == 0x22 );
        REQUIRE( v.data()[3] == 0x33 );

        // little-endian

        number.endianess = little_endian;

        v.clear();
        add_number_to_vector_char(v, number);

        REQUIRE( v.size() == 4 );
        REQUIRE( v.data()[0] == 0x33 );
        REQUIRE( v.data()[1] == 0x22 );
        REQUIRE( v.data()[2] == 0x11 );
        REQUIRE( v.data()[3] == 0x00 );
   }
}
