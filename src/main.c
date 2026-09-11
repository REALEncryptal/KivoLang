#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include "lexer/lexer.h"

int main (void) {
    printf("[===== KIVO =====]\n");

    char *source = get_source("input.kvo");

    printf("here is the first letter: %c\n", *source);

    printf("-------[ test ]-------\n");
    testing();

    return 0;
}
