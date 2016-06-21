#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <iostream>
#include <sstream>
#include "BinMaker.h"


TEST_CASE( "Check fail to make binary", "[binmake]" )
{

    SECTION( "- don't get binary if no input available" )
    {
        BinMaker b;
        std::vector<char> output;
        REQUIRE( b.get_binary(output) == false );
    }

}

TEST_CASE( "Check succeed to make binary", "[binmake]" )
{

    BinMaker b;
    std::stringstream ss;
    std::vector<char> output;
    ss << "big-endian" << std::endl
            << "00010203" << std::endl
            << "little-endian" << std::endl
            << "04050607" << std::endl;
    b.set_input(ss);

    SECTION( "- make a binary from a stringstream" )
    {
        REQUIRE( b.make_binary() == true );
    }

    SECTION( "- made binary endianess valid" )
    {
        REQUIRE( b.get_binary(output) == true );
        REQUIRE( output.size() == 8);
        REQUIRE( output[0] == 0x00 );
        REQUIRE( output[1] == 0x01 );
        REQUIRE( output[2] == 0x02 );
        REQUIRE( output[3] == 0x03 );
        REQUIRE( output[4] == 0x07 );
        REQUIRE( output[5] == 0x06 );
        REQUIRE( output[6] == 0x05 );
        REQUIRE( output[7] == 0x04 );
    }

    SECTION( "- make a binary from an explicit hexadecimal number" )
    {
        ss.str("");
        ss.clear();
        ss << "big-endian" << std::endl <<
                "%x12345678" << std::endl <<
                "%x2f %x00" << std::endl;
        b.set_input(ss);
        b.reset_output();
        REQUIRE( b.make_binary() == true );
        REQUIRE( b.get_binary(output) == true );
        REQUIRE( output.size() == 6);
        REQUIRE( output[0] == 0x12 );
        REQUIRE( output[1] == 0x34 );
        REQUIRE( output[2] == 0x56 );
        REQUIRE( output[3] == 0x78 );
        REQUIRE( output[4] == 0x2f );
        REQUIRE( output[5] == 0x00 );
    }
    //TODO test hex zero padded size
}
