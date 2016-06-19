//============================================================================
// Name        : builtbin.cpp
// Author      : Adel Daouzli
// Version     :
// Copyright   : MIT License
// Description : Make binary file
//============================================================================

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "BinMaker.h"

using namespace std;

void usage(std::string name)
{
    cerr << "Usage: " << name << " input_text_file [output_binary_file]" <<
            endl;
}

int main(int argc, char** argv)
{
    BinMaker b;
    vector<char> bin;
    stringstream ss;
    //b.set_verbosity(true);
    //TODO :
    // if an argument (not option) set input file instead of stdin
    // if option -o set output file instead of stdout
    // if option -v set verbose mode
    if ((argc > 1) && (argc <= 3))
    {
        // read input data from file
        ifstream f(argv[1]);
        if (f.is_open())
        {
            ss << f.rdbuf();
            b.set_input(ss);
            b.make_binary();
            b.get_binary(bin);
            if (argc == 3)
            {
                // write output data to file
                ofstream t(argv[2]);
                t.write(bin.data(), bin.size());
                t.close();
            }
            else
            {
                // write output data to stdout
                for (size_t i = 0; i < bin.size(); ++i)
                {
                    std::cout << bin.data()[i];
                }
                std::cout.flush();
            }
        }
    }
    else if(argc == 1)
    {
        // read input data from stdin
        ss << cin.rdbuf();
        b.set_input(ss);
        b.make_binary();
        b.get_binary(bin);
        // write output data to stdout
        for (size_t i = 0; i < bin.size(); ++i)
        {
            std::cout << bin.data()[i];
        }
        std::cout.flush();
    }
    else
    {
        cerr << "Bad number of arguments" << endl;
        usage(argv[0]);
    }

    return 0;
}
