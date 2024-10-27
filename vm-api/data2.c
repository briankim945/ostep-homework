#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int *data; // Create integer array

    data = malloc(100 * sizeof(int)); // malloc space for data

    data[100] = 0;

    free(data);

    printf("%i\n", data[50]);

    return 0;
}
