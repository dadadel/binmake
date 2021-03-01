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

    SECTION( "- make a binary from a float number" )
    {
        b << "big-endian" <<
                "%f1.2345[4]";
        REQUIRE( b.output_ready());
        REQUIRE( b.size() == 4);
        REQUIRE( b[0] == 0x3f );
        REQUIRE( b[1] == (char)0x9e );
        REQUIRE( b[2] == 0x04 );
        REQUIRE( b[3] == 0x19 );
    }

    SECTION( "test example.txt data conversion" )
    {
        BinStream b;
        uint8_t example_bin[43] = {0x00, 0x11, 0x22, 0x33, 0x4d, 0xe0, 0x77, 0x66,
                              0x55, 0x44, 0x88, 0x99, 0xaa, 0xbb, 0x7b, 0x7b,
                              0x00, 0x00, 0x00, 0x74, 0x68, 0x69, 0x73, 0x20,
                              0x69, 0x73, 0x20, 0x73, 0x6f, 0x6d, 0x65, 0x20,
                              0x72, 0x61, 0x77, 0x20, 0x73, 0x74, 0x72, 0x69,
                              0x6e, 0x67, 0xff};

        b << "# an example of file description of binary data to generate\n"
            << "\n"
            << "# set endianess to big-endian\n"
            << "big-endian\n" << "\n"
            << "# default number is hexadecimal\n"
            << "00112233\n" << "\n"
            << "# man can explicit a number type: %b means binary number\n"
            << "%b0100110111100000\n" << "\n"
            << "# change endianess to little-endian\n"
            << "little-endian\n" << "\n"
            << "# if no explicit, use default\n"
            << "44556677\n" << "\n"
            << "# bytes are not concerned by endianess\n"
            << "88 99 aa bb\n" << "\n"
            << "# change default to decimal\n"
            << "decimal\n" << "\n"
            << "# following number is now decimal on one byte due to its value\n"
            << "0123\n" << "\n"
            << "# following number is now decimal forced to 4 bytes\n"
            << "0123[4]\n" << "\n"
            << "# strings are delimited by \" or '\n"
            << "\"this is some raw string\"\n" << "\n"
            << "# explicit hexa number starts with %x\n"
            << "%xff\n";
        for (int i = 0; i < 43; i++)
        {
            REQUIRE( (uint8_t)b[i] == example_bin[i] );
        }
    }
}
