#include <stdio.h>

int main(int argc, char *argv[])
{
    int *integer; // Create pointer to an integer

    integer = NULL; // Set pointer to null

    printf("%i\n", *integer); // Try to dereference pointer

    return 0;
}
