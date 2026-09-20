#include <corecrt_search.h>
#include <stdlib.h>
#include <string.h>
#include "source.h"
#include "lexer/token.h"

// source readers
char *read_token(const char *source, Token token) {
    // a million ways this can error TODO: add some sanity checks
    char *text = malloc(token.length + 1); // +1 for the string end
    memcpy(text, source + token.start, token.length);

    if (text == NULL) return NULL;

    text[token.length] = '\0';

    return text;
}

int read_token_int(const char *source, Token token) {
    char *text = read_token(source, token);

    if (text == NULL) return NULL;

    return atoi(text);
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
