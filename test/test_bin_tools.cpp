#include "catch.hpp"
#include "bs_data.h"
#include "bin_tools.h"

using namespace std;
using namespace BS;

TEST_CASE("Test of bin_tools functions")
{
    SECTION("Get and check the type of an element")
    {
        string s;

        // Strings

        s = "'blabla'";
        REQUIRE( get_type(s) == t_string );

        s = "\"blabla\"";
        REQUIRE( get_type(s) == t_string );

        // Explicit numbers

        s = "%x023";
        REQUIRE( get_type(s) == t_num_hexadecimal );
        REQUIRE( check_grammar(s, t_num_hexadecimal) );
        s = "%x0a3";
        REQUIRE( check_grammar(s, t_num_hexadecimal) );

        s = "%d-023";
        REQUIRE( get_type(s) == t_num_decimal );
        REQUIRE( check_grammar(s, t_num_decimal) );
        s = "%d0a3";
        REQUIRE( check_grammar(s, t_num_decimal) == false );

        s = "%o023";
        REQUIRE( get_type(s) == t_num_octal );
        REQUIRE( check_grammar(s, t_num_octal) );
        s = "%o09";
        REQUIRE( check_grammar(s, t_num_octal) == false );

        s = "%b01101";
        REQUIRE( get_type(s) == t_num_binary );
        REQUIRE( check_grammar(s, t_num_binary) );
        s = "%b01102";
        REQUIRE( check_grammar(s, t_num_binary) == false );

        s = "some text inside 1 string";
        REQUIRE( get_type(s) == t_none );

        s = "%0101";
        REQUIRE( get_type(s) == t_error );

        // Actions

        REQUIRE( get_type("big-endian") == t_action );
        REQUIRE( is_action("big-endian") );
        REQUIRE( is_action("little-endian") );
        REQUIRE( get_type("size[5]") == t_action );
        REQUIRE( is_action("size[5]") );
        REQUIRE( is_action("hexadecimal") );
        REQUIRE( get_type("hexa") == t_action );
        REQUIRE( is_action("hexa") );
        REQUIRE( is_action("hex") );
        REQUIRE( is_action("decimal") );
        REQUIRE( is_action("octal") );
        REQUIRE( is_action("oct") );
        REQUIRE( get_type("binary") == t_action );
        REQUIRE( is_action("binary") );
        REQUIRE( is_action("bin") );
        REQUIRE( is_action("blabla") == false );
    }

    SECTION("Build numbers")
    {
        string s;
        number_t num;

        s = "-42";
        REQUIRE( build_number(s, num, t_num_decimal, big_endian, 4) );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 4 );
        REQUIRE( num.type == t_32bits );
        REQUIRE( num.num_signed );
        REQUIRE( num.value_i64 == -42 );

        s = "42";
        REQUIRE( build_number(s, num, t_num_decimal, big_endian) );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 1 );
        REQUIRE( num.type == t_8bits );
        REQUIRE( !num.num_signed );
        REQUIRE( num.value_u64 == 42 );

        s = "%xa42";
        REQUIRE( build_number(s, num, t_num_hexadecimal, big_endian) );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 2 );
        REQUIRE( num.type == t_16bits );
        REQUIRE( !num.num_signed );
        REQUIRE( num.value_u64 == 0x0a42 );

        REQUIRE( build_number(s, num, t_num_hexadecimal, little_endian) );
        REQUIRE( num.endianess == little_endian );
        REQUIRE( num.value_u64 == 0x0a42 );

        s = "000a42";
        REQUIRE( build_number(s, num, t_num_hexadecimal, big_endian) );
        REQUIRE( num.endianess == big_endian );
        REQUIRE( num.size == 4 );
        REQUIRE( num.type == t_32bits );
        REQUIRE( !num.num_signed );
        REQUIRE( num.value_u64 == 0x0a42 );
    }
}
