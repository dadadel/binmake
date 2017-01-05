//============================================================================
// Name        : binmake.cpp
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

static const string __version("V0.1");

void usage(std::string name)
{
    cerr << "BinMake " << __version << " - Adel Daouzli - MIT License" << endl
            << endl << "Usage: " << endl
            << "\t" << name << " [options] [text_file [binary_file]]" << endl
            << endl << "Description:" << endl
            << "\tGenerates binary data from a text human readable description." << endl
            << "\tIf `binary_file` is not provided then the output will be stdout" << endl
            << "\t(unless using option -o)." << endl
            << "\tIf `text_file` is not provided then the input will be stdin." << endl
            << endl << "Options:" << endl
            << "\t-h : show this help message and exit" << endl
            << "\t-v : activate verbose mode" << endl
            << "\t-o binary_file : will generate the binary output to the "
            << "provided file name" << endl;
}

int main(int argc, char** argv)
{
    BinStream b;
    string output_file;
    int argoffs = 0;

    // Manage options
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            argoffs++;
            // set verbose mode with -v
            if (argv[i][1] == 'v')
            {
                b.set_verbosity(true);
            }
            // show help and exit with -h
            else if (argv[i][1] == 'h')
            {
                usage(argv[0]);
                return 0;
            }
            // get output file name with -o FILENAME
            else if (argv[i][1] == 'o')
            {
                i++;
                argoffs++;
                output_file = argv[i];
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
        if ((argc == 3) || (!output_file.empty()))
        {
            // write output data to file
            if (output_file.empty())
            {
                output_file = argv[argoffs + 2];
            }
            ofstream t(output_file.c_str());
            b >> t;
            t.close();
        }
        else
        {
            // write output data to stdout
            cout << b;
        }
    }
    else if(argc == 1)
    {
        // read input data from stdin
        cin >> b; // can work also with b << cin;
        if (output_file.empty())
        {
            // write output data to stdout
            cout << b;
        }
        else
        {
            // write output data to file
            ofstream t(output_file.c_str());
            b >> t;
            t.close();
        }
    }
    else
    {
        cerr << "Bad number of arguments" << endl;
        usage(argv[0]);
    }

    return 0;
}
