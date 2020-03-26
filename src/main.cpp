#include "Process.h"
#include <iostream>

int main()
{
    Process test("test.out");

    if(test.write("Hello", 5) < 0)
    {
        std::cerr << "problem with write to procces\n";
        return 0;
    }

    char read[256];
    if(test.read(read, 256))
    {
        std::cerr << "problem with write to procces\n";
        return 0;
    }

    test.close();
    return 0;
}

//

