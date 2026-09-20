#ifndef KIVO_PARSER_H
#define KIVO_PARSER_H

#include <stdlib.h>
#include "../lexer/token.h"
#include "source.h"
#include "node.h"

typedef struct {
    Node root; 
    Token *tokens;
    const char* source;
    size_t start;
    size_t current;
    char *error_message;
} Parser;

// parsers
int parse_program(Parser *parser);
int parse_let_statement(Parser *parser, Node *statement);
int parse_expression(Parser *parser, Node *expression);
int parse_statement(Parser *parser, Node *statement);
int parse_primary(Parser *parser, Node *primary);

// parser utils
void append_root_statement(Parser *parser, Node node);

Token peek(Parser *parser);
Token peek_next(Parser *parser);
Token peek_previous(Parser *parser);
int is_at_end(Parser *parser);
Token advance(Parser *parser);
Token skip_seperators(Parser *parser);
int is_type(Parser *parser, TokenType type);
Token consume(Parser *parser, TokenType type);
int expect(Parser *parser, Token *result, TokenType type);

#endif // KIVO_PARSER_H