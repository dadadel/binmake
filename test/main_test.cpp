#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include "BinStream.h"

using namespace BS;
using namespace std;


TEST_CASE("REGEX methods")
{
    SECTION("Get the type of an element")
    {
        BinStream b;
        string s;

        s = "'blabla'";
        REQUIRE( b.get_type(s) == t_string );

        s = "\"blabla\"";
        REQUIRE( b.get_type(s) == t_string );

        // Explicit numbers

        s = "%x023";
        REQUIRE( b.get_type(s) == t_num_hexadecimal );
        s = s.substr(2, s.size() - 2);
        cout<<"s="<<s<<endl;
        REQUIRE( b.check_grammar(s, t_num_hexadecimal) );

        s = "%d-023";
        REQUIRE( b.get_type(s) == t_num_decimal );
        s = s.substr(2, s.size() - 2);
        REQUIRE( b.check_grammar(s, t_num_decimal) );

        s = "%o023";
        REQUIRE( b.get_type(s) == t_num_octal );
        s = s.substr(2, s.size() - 2);
        REQUIRE( b.check_grammar(s, t_num_octal) );

        s = "%b01101";
        REQUIRE( b.get_type(s) == t_num_binary );
        s = s.substr(2, s.size() - 2);
        REQUIRE( b.check_grammar(s, t_num_binary) );

        s = "%0101";
        REQUIRE( b.get_type(s) == t_error );

        // default number is hexa
        s = "023";
        REQUIRE( b.get_type(s) == t_num_hexadecimal );
    }
}

TEST_CASE( "Check failure to make binary", "[binstream]" )
{

    SECTION( "- don't get binary if no input available" )
    {
        BinStream b;
        vector<char> output;
        REQUIRE( b.get_output(output) == false );
    }

}

TEST_CASE( "Check success to make binary", "[binstream]" )
{

    BinStream b;
    stringstream ss;
    vector<char> output;
    ss << "big-endian"
        << " 00010203"
        << " little-endian"
        << " 04050607";

    SECTION( "- get input from a stringstream" )
    {
        b << ss;
        REQUIRE( b.input_ready() == true );
    }

    SECTION( "- produced an output binary from a stringstream" )
    {
        b << ss;
        REQUIRE( b.output_ready() == true );
    }

    SECTION( "- made binary endianess valid" )
    {
        b << ss;
        REQUIRE( b.size() == 8);
        REQUIRE( b[0] == 0x00 );
        REQUIRE( b[1] == 0x01 );
        REQUIRE( b[2] == 0x02 );
        REQUIRE( b[3] == 0x03 );
        REQUIRE( b[4] == 0x07 );
        REQUIRE( b[5] == 0x06 );
        REQUIRE( b[6] == 0x05 );
        REQUIRE( b[7] == 0x04 );
    }

    SECTION( "- reset the data" )
    {
        b << ss;
        b.reset();
        REQUIRE( b.input_ready() == false);
        REQUIRE( b.output_ready() == false);
    }

    SECTION( "- make a binary from an explicit hexadecimal number" )
    {
        b << "big-endian" <<
                "%x12345678" <<
                "%x2f %x00";
        REQUIRE( b.size() == 6);
        REQUIRE( b[0] == 0x12 );
        REQUIRE( b[1] == 0x34 );
        REQUIRE( b[2] == 0x56 );
        REQUIRE( b[3] == 0x78 );
        REQUIRE( b[4] == 0x2f );
        REQUIRE( b[5] == 0x00 );
    }
    //TODO test hex zero padded size
}
