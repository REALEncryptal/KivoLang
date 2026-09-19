#include "parser.h"
#include "main.h"
#include "node.h"
#include "../lexer/token.h"
#include <stdio.h>
#include <stdlib.h>




void parse(Token *tokens) {
    Node root = {NODE_CHUNK}; 
    size_t node_count = 0;

    Parser parser = {
        tokens,
        0,0
    };


    while (1) {
        parser.start = parser.current;
        Token token = tokens[parser.current];

        

        parser.current ++;
    }
}

// parsers


// node constructors

Node binary_expr(Token operator, Token left, Token right) {
    SourcePosition position = get_position_range(left,right);

    Node node = {
        NODE_BINARY_EXPRESSION,
        position
    };

    node.as.binary_expr.left = parse_expression();
}

Node literal_expr(Token literal, TokenType type) {
    return 
}

// utility

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

Token peek(Parser *parser) {
    return parser->tokens[parser->current];
}

Token previous(Parser *parser) {
    return parser->tokens[parser->current - 1];
}

int is_at_end(Parser *parser) {
    return peek(parser).type == TOKEN_EOF;
}

Token advance(Parser *parser) {
    if (!is_at_end(parser)) (parser->current)++;

    return previous(parser);
}

int is_type(Parser *parser, TokenType type) {
    return peek(parser).type == type;
}

Token consume(Parser *parser, TokenType type) {
    if (is_type(parser, type)) return advance(parser);

    // TODO: proper error handling
    printf("Expected token of type %-22s", token_names[type]);
    return (Token) {TOKEN_ERROR};
}

