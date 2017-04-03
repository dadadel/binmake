#include <string>
#include <vector>

#include "catch.hpp"
#include "bs_data.h"
#include "bin_tools.h"

using namespace std;
using namespace BS;

TEST_CASE("Unit Tests of bin_tools functions")
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
        REQUIRE( get_type("%f12.5") == t_num_float );
        REQUIRE( get_type("%f-0.12345") == t_num_float );

        // Not valid number

        REQUIRE( get_type("%0101") == t_error );
        REQUIRE( get_type("%d-023") == t_num_decimal );

        // Internal states

        REQUIRE( get_type("big-endian") == t_internal_state );
        REQUIRE( get_type("size[5]") == t_internal_state );
        REQUIRE( get_type("hexa") == t_internal_state );
        REQUIRE( get_type("binary") == t_internal_state );
    }

    SECTION("Unit test of 'is_internal_state()'")
    {
        // Endianess

        REQUIRE( is_internal_state("big-endian") );
        REQUIRE( is_internal_state("little-endian") );

        // Size

        REQUIRE( is_internal_state("size[5]") );

        // Numbers base

        REQUIRE( is_internal_state("hexadecimal") );
        REQUIRE( is_internal_state("hexa") );
        REQUIRE( is_internal_state("hex") );
        REQUIRE( is_internal_state("decimal") );
        REQUIRE( is_internal_state("octal") );
        REQUIRE( is_internal_state("oct") );
        REQUIRE( is_internal_state("binary") );
        REQUIRE( is_internal_state("bin") );

        // Not valid

        REQUIRE( is_internal_state("blabla") == false );
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
        REQUIRE( check_grammar("%f12.5", t_num_float) );
        REQUIRE( check_grammar("%f-0.12345", t_num_float) );
        REQUIRE( check_grammar("%f5.e3", t_num_float) );
        REQUIRE( check_grammar("%f5.145E-3", t_num_float) );
        REQUIRE( check_grammar("%f5.145e+10", t_num_float) );
        REQUIRE( check_grammar("%f5", t_num_float) );

        // Not valid grammar
        REQUIRE( check_grammar("%x0g23", t_num_hexadecimal) == false );
        REQUIRE( check_grammar("%d0a3", t_num_decimal) == false );
        REQUIRE( check_grammar("%o09", t_num_octal) == false );
        REQUIRE( check_grammar("%b01102", t_num_binary) == false );
        REQUIRE( check_grammar("%f1a.2", t_num_float) == false );

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
        REQUIRE( check_grammar("%f-2.e8[4]", t_num_float) );
        REQUIRE( check_grammar("%f23.5[8]", t_num_float) );
    }

    SECTION("Unit test of 'extract_number()'")
    {
        string s;
        number_t num;

        s = "-42";
        REQUIRE( extract_number(s, num, t_num_decimal, big_endian, 4) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 4 );
        REQUIRE( num.num_signed );
        REQUIRE( num.value_i64 == -42 );

        s = "42";
        REQUIRE( extract_number(s, num, t_num_decimal, big_endian) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 1 );
        REQUIRE( !num.num_signed );
        REQUIRE( num.value_u64 == 42 );

        s = "%xa42";
        REQUIRE( extract_number(s, num, t_num_hexadecimal, big_endian) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 2 );
        REQUIRE( !num.num_signed );
        REQUIRE( num.value_u64 == 0x0a42 );

        REQUIRE( extract_number(s, num, t_num_hexadecimal, little_endian) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == little_endian );
        REQUIRE( num.value_u64 == 0x0a42 );

        s = "000a42";
        REQUIRE( extract_number(s, num, t_num_hexadecimal, big_endian) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 4 );
        REQUIRE( !num.num_signed );
        REQUIRE( num.value_u64 == 0x0a42 );

        s = "%f-2.5";
        REQUIRE( extract_number(s, num, t_num_float, big_endian, 4) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 4 );
        REQUIRE( num.value_f32 == -2.5 );

        s = "%f1.2345";
        REQUIRE( extract_number(s, num, t_num_float, big_endian, 4) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 4 );
        REQUIRE( num.value_f32 == 1.2345f );

        s = "%f14.23e-4[8]";
        REQUIRE( extract_number(s, num, t_num_float, little_endian, 8) );
        REQUIRE( num.is_set );
        REQUIRE( num.endianess == little_endian );
        REQUIRE( num.size == 8 );
        REQUIRE( num.value_f64 == 14.23e-4 );
}

    SECTION("Unit test of 'add_number_to_vector_char()'")
    {
        vector<char> v;
        number_t number;
        number.is_set = true;
        number.size = 4;
        number.num_signed = false;
        number.value_u64 = 0x00112233;

        // little-endian

        number.endianess = little_endian;

        add_number_to_vector_char(v, number);

        REQUIRE( v.size() == 4 );
        REQUIRE( v.data()[0] == 0x33 );
        REQUIRE( v.data()[1] == 0x22 );
        REQUIRE( v.data()[2] == 0x11 );
        REQUIRE( v.data()[3] == 0x00 );

        // big-endian

        number.endianess = big_endian;

        v.clear();
        add_number_to_vector_char(v, number);

        REQUIRE( v.size() == 4 );
        REQUIRE( v.data()[0] == 0x00 );
        REQUIRE( v.data()[1] == 0x11 );
        REQUIRE( v.data()[2] == 0x22 );
        REQUIRE( v.data()[3] == 0x33 );

        // float number

        number.value_f32 = 1.2345;
        v.clear();
        add_number_to_vector_char(v, number);

        REQUIRE( v.size() == 4 );
        REQUIRE( v.data()[0] == 0x3F );
        REQUIRE( v.data()[1] == (char)0x9E );
        REQUIRE( v.data()[2] == 0x04 );
        REQUIRE( v.data()[3] == 0x19 );
   }

    SECTION("Unit test of 'extract_endianess()'")
    {
        endianess_t endian;

        REQUIRE ( extract_endianess("little-endian", endian) );
        REQUIRE ( endian == little_endian);
        REQUIRE ( extract_endianess("big-endian", endian) );
        REQUIRE ( endian == big_endian);

        REQUIRE ( extract_endianess("little_endian", endian) == false );
        REQUIRE ( extract_endianess("little endian", endian) == false );
        REQUIRE ( extract_endianess("middle-endian", endian) == false );
        REQUIRE ( extract_endianess("blabla", endian) == false );
    }

    SECTION("Unit test of 'extract_number_type()'")
    {
        type_t num_type;

        REQUIRE( extract_number_type("hexadecimal", num_type) );
        REQUIRE( num_type == t_num_hexadecimal );
        REQUIRE( extract_number_type("hexa", num_type) );
        REQUIRE( num_type == t_num_hexadecimal );
        REQUIRE( extract_number_type("hex", num_type) );
        REQUIRE( num_type == t_num_hexadecimal );
        REQUIRE( extract_number_type("decimal", num_type) );
        REQUIRE( num_type == t_num_decimal );
        REQUIRE( extract_number_type("dec", num_type) );
        REQUIRE( num_type == t_num_decimal );
        REQUIRE( extract_number_type("octal", num_type) );
        REQUIRE( num_type == t_num_octal );
        REQUIRE( extract_number_type("oct", num_type) );
        REQUIRE( num_type == t_num_octal );
        REQUIRE( extract_number_type("binary", num_type) );
        REQUIRE( num_type == t_num_binary );
        REQUIRE( extract_number_type("bin", num_type) );
        REQUIRE( num_type == t_num_binary );
        REQUIRE( extract_number_type("float", num_type) );
        REQUIRE( num_type == t_num_float );

        REQUIRE( extract_number_type("hexadec", num_type) == false );
        REQUIRE( extract_number_type("blabla", num_type) == false );
        REQUIRE( extract_number_type("b", num_type) == false );
    }

    SECTION("Unit test of 'get_state_type()'")
    {
        state_type_t state;

        REQUIRE( get_state_type("little-endian", state) );
        REQUIRE( state == t_state_type_endianess );
        REQUIRE( get_state_type("big-endian", state) );
        REQUIRE( state == t_state_type_endianess );

        REQUIRE( get_state_type("hexadecimal", state) );
        REQUIRE( state == t_state_type_number );
        REQUIRE( get_state_type("hexa", state) );
        REQUIRE( state == t_state_type_number );
        REQUIRE( get_state_type("hex", state) );
        REQUIRE( state == t_state_type_number );
        REQUIRE( get_state_type("decimal", state) );
        REQUIRE( state == t_state_type_number );
        REQUIRE( get_state_type("dec", state) );
        REQUIRE( state == t_state_type_number );
        REQUIRE( get_state_type("octal", state) );
        REQUIRE( state == t_state_type_number );
        REQUIRE( get_state_type("oct", state) );
        REQUIRE( state == t_state_type_number );
        REQUIRE( get_state_type("binary", state) );
        REQUIRE( state == t_state_type_number );
        REQUIRE( get_state_type("bin", state) );
        REQUIRE( state == t_state_type_number );
        REQUIRE( get_state_type("float", state) );
        REQUIRE( state == t_state_type_number );

        REQUIRE( get_state_type("size[2]", state) );
        REQUIRE( state == t_state_type_size );
        REQUIRE( get_state_type("size[0]", state) );
        REQUIRE( state == t_state_type_size );

        REQUIRE( get_state_type("size", state) == false );
        REQUIRE( get_state_type("middle-endian", state) == false );
        REQUIRE( get_state_type("blabla", state) == false );
        REQUIRE( get_state_type("deci", state) == false );
    }

    SECTION("Unit test of 'extract_size()'")
    {
        int size;

        REQUIRE( extract_size("size[1]", size) );
        REQUIRE( size == 1 );
        REQUIRE( extract_size("size[2]", size) );
        REQUIRE( size == 2 );
        REQUIRE( extract_size("size[4]", size) );
        REQUIRE( size == 4 );
        REQUIRE( extract_size("size[8]", size) );
        REQUIRE( size == 8 );
        REQUIRE( extract_size("size[0]", size) );
        REQUIRE( size == 0 );

        REQUIRE( extract_size("size[16]", size) == false);
        REQUIRE( extract_size("size[3]", size) == false);
        REQUIRE( extract_size("size[5]", size) == false);
        REQUIRE( extract_size("size[6]", size) == false);
        REQUIRE( extract_size("size[]", size) == false);
        REQUIRE( extract_size("size [1]", size) == false);
        REQUIRE( extract_size("size 1", size) == false);
        REQUIRE( extract_size("size1", size) == false);
    }
}
