/*
 * test_binstream.cpp
 *
 *  Created on: 2 janv. 2017
 *      Author: dragon
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "BinStream.h"

using namespace std;

int main()
{
    BS::BinStream b;
    vector<char> bin;
    stringstream ss;

    // from input file to output file
    ifstream f("example.txt");
    ofstream t("example_bs.bin");
    b << f >> t;
    f.close();
    t.close();

    // from strings to output file
    b.reset();
    b << "'hello world!'"
            << "00112233"
            << "big-endian"
            << "00112233"
            << "decimal"
            << "00112233"
            << "\"ok\"";
    t.open("ex2.bin");
    b >> t;
    t.close();
    return 0;
}
