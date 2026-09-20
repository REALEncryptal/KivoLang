#ifndef KIVO_SOURCE_H
#define KIVO_SOURCE_H

#include "lexer/token.h"
#include <stdlib.h>

typedef struct {
    size_t start;
    size_t length;
} SourcePosition;

int read_token_int(const char *source, Token token);
char *read_token(const char *source, Token token);

SourcePosition get_position_range(Token start, Token end);
SourcePosition get_position(Token token);

#endif // KIVO_SOURCE_H