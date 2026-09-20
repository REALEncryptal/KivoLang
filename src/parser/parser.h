#ifndef KIVO_PARSER_H
#define KIVO_PARSER_H

#include <stdlib.h>
#include "../lexer/token.h"
#include "source.h"
#include "node.h"

typedef struct {
    Token *tokens;
    const char* source;
    size_t start;
    size_t current;
    int had_error;
} Parser;

// parsers
Node *parse_primary(Parser *parser, Token *token);

// parser utils
Token peek(Parser *parser);
Token previous(Parser *parser);
int is_at_end(Parser *parser);
Token advance(Parser *parser);
int is_type(Parser *parser, TokenType type);
Token consume(Parser *parser, TokenType type);

#endif // KIVO_PARSER_H