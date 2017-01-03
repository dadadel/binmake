//============================================================================
// Name        : builtbin.cpp
// Author      : Adel Daouzli
// Version     :
// Copyright   : MIT License
// Description : Make binary file
//============================================================================

#include <iostream>
#include <fstream>

#include "BinStream.h"

using namespace std;
using namespace BS;

void usage(std::string name)
{
    cerr << "Usage: " << name << " input_text_file [output_binary_file]" <<
            endl;
}

int main(int argc, char** argv)
{
    BinStream b;
    int argoffs = 0;

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            argoffs++;
            if (argv[i][1] == 'v')
            {
                b.set_verbosity(true);
            }
            else
            {
                cerr << "WARNING: Unexpected option '" << argv[i] << "'" << endl;
            }
        }
    }
    argc -= argoffs;

    if ((argc > 1) && (argc <= 3))
    {
        // read input data from file
        ifstream f(argv[argoffs + 1]);
        b << f;
        if (argc == 3)
        {
            // write output data to file
            ofstream t(argv[argoffs + 2]);
            b >> t;
            t.close();
        }
        else
        {
            // write output data to stdout
            for (size_t i = 0; i < b.size(); ++i)
            {
                cout << b[i];
            }
            cout.flush();
        }
    }
    else if(argc == 1)
    {
        // read input data from stdin
        b << cin;
        // write output data to stdout
        for (size_t i = 0; i < b.size(); ++i)
        {
            cout << b[i];
        }
        cout.flush();
    }
    else
    {
        cerr << "Bad number of arguments" << endl;
        usage(argv[0]);
    }

    return 0;
}
