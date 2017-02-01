#include <string>
#include <vector>

#include "catch.hpp"
#include "BinStream.h"

using namespace std;
using namespace BS;

TEST_CASE("Unit Tests of issues")
{
    SECTION("Test issue #1 - 1 byte")
    {
        BinStream b;
        
        b << "decimal" << "26[1]";
        REQUIRE( b.size() == 1 );
        REQUIRE( b[0] == 0x1a );
    }

    SECTION("Test issue #1 - 2 bytes")
    {
        BinStream b;
        
        b << "decimal" << "26[2]";
        REQUIRE( b.size() == 2 );
        REQUIRE( b[0] == 0x1a );
        REQUIRE( b[1] == 0x00 );
    }
}
