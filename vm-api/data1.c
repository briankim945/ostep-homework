#include <stdlib.h>

int main(int argc, char *argv[])
{
    int *data; // Create integer array

    data = malloc(100 * sizeof(int)); // malloc space for data

    data[100] = 0;

    return 0;
}
