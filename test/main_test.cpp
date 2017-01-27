#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include "BinStream.h"
#include "bin_tools.h"

using namespace BS;
using namespace std;

TEST_CASE( "Check internal", "[binstream]" )
{
    SECTION ( "Unit test of 'update_internal_state'")
    {
        BinStream b;
        b << "little-endian 0001";
        REQUIRE( b.update_internal_state("big-endian") );
        b << " 0203";
        REQUIRE( b.size() == 4);
        REQUIRE( b[0] == 0x01 );
        REQUIRE( b[1] == 0x00 );
        REQUIRE( b[2] == 0x02 );
        REQUIRE( b[3] == 0x03 );

    }

    SECTION ( "Unit test of 'proceed_input'")
    {
        BinStream b(true);
        string s;
        
        s = "little-endian 0001\n# this is a comment\nbig-endian 0203\n'abc'";
        b.proceed_input(s);
        vector<char> output;
        REQUIRE( b.size() == 7);
        REQUIRE( b[0] == 0x01 );
        REQUIRE( b[1] == 0x00 );
        REQUIRE( b[2] == 0x02 );
        REQUIRE( b[3] == 0x03 );
        REQUIRE( b[4] == 'a' );
        REQUIRE( b[5] == 'b' );
        REQUIRE( b[6] == 'c' );
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
