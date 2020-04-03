#include <unistd.h>
#include <cstdio>

int main()
{
    char buffer[256];
    while (!feof(stdin))
    {
        size_t bytes = fread(buffer, sizeof(char), 5, stdin);
        //fprintf(stderr, "Yeh %s", buffer);
        fwrite(buffer, sizeof(char), bytes, stdout);
        fprintf(stderr, "Yeh %s %d", buffer, bytes);
    }

    return 0;
}