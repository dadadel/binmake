# binmake

This is an open source tool for generating a binary file described by a human readable text file.
It manage endianess, several numbers representations, raw strings.
It ignores comments lines, empty lines, leading and ending spaces.

## How to install

`binmake` is a standalone binary written in C++ with C++11 standard.
You should build the binary and then use it.

You can also compile the module as static or dynamic library to use the streaming
class BS::BinStream in your own code.

    $ make

## How to use

### using the binary

    $ ./binmake exemple.txt exemple.bin

    $ ./makebin exemple.txt > exemple.bin

    $ cat exemple.txt | ./makebin > exemple.bin

    $ echo '32 decimal 32 %x61 61' | ./binmake | hexdump -C
    00000000  32 20 61 3d                                       |2 a=|
    00000004

- Input file `exemple.txt`:

```
# an exemple of file description of binary data to generate

# set endianess to big-endian
big-endian

# default number is hexadecimal
00112233

# man can explicit a number type: %b means binary number
%b0100110111100000

# change endianess to little-endian
little-endian

# if no explicit, use default
44556677

# bytes are not concerned by endianess
88 99 aa bb

# change default to decimal
decimal

# following number is now decimal
0123

# strings are delimited by " or '
"this is some raw string"

# explicit hexa number starts with %x
%xff
```

- Output file`exemple.bin`:

```
00000000  00 11 22 33 4d e0 77 66  55 44 88 99 aa bb 7b 74  |.."3M.wfUD....{t|
00000010  68 69 73 20 69 73 20 73  6f 6d 65 20 72 61 77 20  |his is some raw |
00000020  73 74 72 69 6e 67 ff                              |string.|
00000027
```

# How to include in your code

The header to include is `BinStream.h`.
The class to use is `BS::BinStream`. To make it easy you can declare the
namespace `BS`.

You can stream the text description of you binary from: `istream`, `ifstream`,
`string` and `stringstream`.

You can stream the output binary to: `vector<char>` and `ofstream`.

There follow some examples.

- Create a binary file from strings

```c++
#include <fstream>
#include "BinStream.h"

using namespace std;
using namespace BS;

int main()
{
    BinStream bin;
    bin << "'hello world!'"
            << "00112233"
            << "big-endian"
            << "00112233";
    ofstream f("test.bin");
    bin >> f;
    return 0;
}
```

- Create a binary file from a text file

```c++
#include <fstream>
#include "BinStream.h"

using namespace std;
using namespace BS;

int main()
{
    BinStream bin;
    ifstream inf("example.txt");
    ofstream ouf("example.bin");
    bin << inf >> ouf;
    return 0;
}
```

- Read from stdin and write to stdout

```c++
#include <iostream>
#include "BinStream.h"

using namespace std;
using namespace BS;

int main()
{
    BinStream bin;
    bin << cin;
    for (size_t i = 0; i < bin.size(); ++i)
    {
        cout << bin[i];
    }
    cout.flush();
    return 0;
}
```

- Get the output in a vector

```c++
#include <iostream>
#include "BinStream.h"

using namespace std;
using namespace BS;

int main()
{
    BinStream bin;
    vector<char> output;
    bin << "'hello world!'" << "00112233";
    bin >> output;
    for (size_t i = 0; i < output.size(); ++i)
    {
        cout << output.data()[i];
    }
    return 0;
}
```

## Brief formating documentation

### Comments

A comment is in a single line and starts with a #. It should be the first
character other than space characters.

### String

A string should start and end by either `"` (double quotes) or `'`
(single quote) on the same line.

### Numbers

It is possible to set on the same line several numbers separated by spaces.
There is several way to represent a number. By default a number is expected to
be in hexadecimal representation unless the default representation is changed.

To force interpretation of a number representation, it should start with one of
the following prefix:
- `%x` represents hexadecimal number (digits in [0-9a-fA-F])
- `%d` represents decimal number (digits in [0-9])
- `%o` represents octal number (digits in [0-7])
- `%b` represents binary number (digits in [0-1])

A number will be then represented as a 8 bits, 16 bits, 32 bits or 64 bits
number in the binary output depending on the value of the number. If exceeding
one of the above bits size it will use the next available size (max 64 bits).

As exception, if a hexadecimal number is provided with non-significant zeros,
the size of the string representation will determine the output binary number
size. Thus, "%x0000" (or "0000" if default is hexa) will generate a 16 bits
number instead of 8 bits.

For the numbers represented on 16, 32 and 64 bits, the default endianess is
little-endian unless it was changed (see Keywords).

### Keywords

Some special keywords can be used to change default endianess output usage or
input not explicit number interpretation.

Note that a keyword should be on a single line without any character other than
space characters.

- `little-endian`, `big-endian`

For the number represented on 16, 32 and 64 bits, the default endianess is
little-endian. The default endianess can be changed by using the keyword
"little-endian" or "big-endian".
Once the default is changed, all further numbers will be represented in the
new default endianess.

As a reminder, big-endian representation of `%x00112233` is in
memory `00 11 22 33`, that is the less significant byte in the lower memory
address.
Conversly, in little-endian, the number will be represented like `33 22 11 00`,
that is the more significant bytes in lower addresses.


- `hexadecimal`, `hexa`, `hex`

If one of these keywords is found, then all furthur found number without an
explicit interpretation will be interpreted as a **hexadecimal number**.

- `decimal`, `dec`

If one of these keywords is found, then all furthur found number without an
explicit interpretation will be interpreted as a **decimal number**.

- `octal`, `oct`

If one of these keywords is found, then all furthur found number without an
explicit interpretation will be interpreted as a **octal number**.

- `binary`, `bin`

If one of these keywords is found, then all furthur found number without an
explicit interpretation will be interpreted as a **binary number**.



