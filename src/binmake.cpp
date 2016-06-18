//============================================================================
// Name        : builtbin.cpp
// Author      : Adel Daouzli
// Version     :
// Copyright   : GPL3
// Description : Built binary file
//============================================================================

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "BinBuilder.h"

using namespace std;

int main(int argc, char** argv)
{
    BinBuidler b;
    vector<char> bin;
    //b.set_verbosity(true);
    //TODO :
    // if an argument (not option) set input file instead of stdin
    // if option -o set output file instead of stdout
    // if option -v set verbose mode
    if ((argc > 1) && (argc <= 3))
    {
        stringstream ss;
        ifstream f(argv[1]);
        ofstream t(argv[2]);
        if (f.is_open())
        {
            ss << f.rdbuf();
            b.set_input(ss);
            b.buildbin();
            b.get_binary(bin);
            t.write(bin.data(), bin.size());
            t.close();
        }
    }
    else
    {
        cerr << "Bad number of arguments" << endl <<
                "Syntax: " << argv[0] <<
                " input_text_file [output_binary_file]" << endl;
    }

    return 0;
}
