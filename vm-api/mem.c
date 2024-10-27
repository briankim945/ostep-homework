#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int *integer; // Create pointer to an integer

    integer = (int *)malloc(sizeof(int)); // malloc value for integer

    // free(integer); // forget to free integer

    return 0;
}
