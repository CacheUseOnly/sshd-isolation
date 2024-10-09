#include <stdio.h>
#include <stdlib.h>

#include "isolation.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("usage: %s (add|head) args\n", argv[0]);
        return -1;
    }

    /* Add two integers */
    if (strcmp(argv[1], "add") == 0) {
        int a, b;

        a = atoi(argv[2]);
        b = atoi(argv[3]);
        printf("%d+%d=%d", a, b, iso_add(a, b));
    
    /* Print the first 8 bytes of a file */
    } else if (strcmp(argv[1], "cat") == 0) {
        printf("first 8 char of %s: %s\n", argv[2], iso_cat(argv[2]));
    }

    return 0;
}