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
    BinStream b;
    vector<char> bin;
	stringstream ss;
    ifstream f("exemple.txt");
    if (f.is_open())
    {
        ss << f.rdbuf();
        b.add_element(ss.str());
        b.get_binary(bin);
        ofstream t("exemple_bs.bin");
		t.write(bin.data(), bin.size());
		t.close();
    }
}
