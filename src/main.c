#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include "lexer/lexer.h"

int main (void) {
    printf("[===== KIVO =====]\n");

    const char *source = get_source("input.kvo");

    printf("here is the first letter: %c\n", *source);

    printf("-------[ test ]-------\n");
    Token *tokens = tokenize(source);
    print_tokens(tokens, source);

    return 0;
}
