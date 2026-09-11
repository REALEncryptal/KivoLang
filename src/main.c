#include "file.h"
#include <stdio.h>
#include <stdlib.h>


int main (void) {
    printf("[===== KIVO =====]\n");

    char *source = getSource("input.kvo");

    printf("here is the first letter: %c\n", *source);

    return 0;
}
