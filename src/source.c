
#include <stdlib.h>
#include <string.h>
#include "source.h"
#include "lexer/token.h"

// source readers
int read_token(const char *source, Token token, char *buffer) {
    // a million ways this can error TODO: add some sanity checks
    char *text = malloc(token.length + 1); // +1 for the string end
    memcpy(text, source + token.start, token.length);

    if (text == NULL) return 0;

    text[token.length] = '\0';

    *buffer = *text;
    return 1;
}

int read_token_int(const char *source, Token token, int *num) {
    char text;
    if (!read_token(source, token, &text)) return 0;

    *num = atoi(&text);
    return 1;
}

// source position
SourcePosition get_position_range(Token start, Token end) {
    return (SourcePosition) {
        start.start,
        end.start + end.length - start.start
    };
}

SourcePosition get_position(Token token) {
    return (SourcePosition) {
        token.start,
        token.length
    };
}
