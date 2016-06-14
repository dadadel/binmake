//============================================================================
// Name        : builtbin.cpp
// Author      : Adel Daouzli
// Version     :
// Copyright   : GPL3
// Description : Built binary file
//============================================================================

#include <iostream>
#include "BinBuidler.h"

using namespace std;

int main()
{
    string s("#commentaire\n\"contenu!\"\nlittle-endian\n \n'0b01100'\n5487\n\n0000abcd\n'fin'");
    BinBuidler b;
    b.set_input(s);
    b.buildbin();
    //cout << s << endl;
    return 0;
}
