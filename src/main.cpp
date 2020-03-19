#include "Process.h"

int main()
{
    Process test("test.out");

    test.write("Hello", 5);

    char read[256];
    test.read(read, 256);

    test.write("Hello2", 5);

    test.close();
    return 0;
}

//

