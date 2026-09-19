#ifndef KIVO_PARSER_H
#define KIVO_PARSER_H

#include <stdlib.h>
#include "../lexer/token.h"

typedef struct {
    Token *tokens;
    size_t start;
    size_t current;
    int had_error;
} Parser;

SourcePosition get_position_range(Token start, Token end)

#endif // KIVO_PARSER_H