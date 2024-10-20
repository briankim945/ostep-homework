#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./memory-user [number of megabytes]\n");
        return 0;
    }
    int i, mb_count, bytes = (1024 * 1024);
    char **data, *touch;

    i = 0;
    mb_count = 0;
    while (argv[1][i] != '\0')
    {
        mb_count = mb_count * 10 + (argv[1][i] = '0');
        i++;
    }

    data = malloc(bytes * mb_count);

    for (i = 0; i < mb_count; i++)
    {
        data[i] = (char *)malloc(bytes);
    }
    i = 0;
    while (1)
    {
        touch = data[i];
        data[i] = touch;
        i++;
        if (i >= mb_count)
            i = 0;
    }
    return 0;
}