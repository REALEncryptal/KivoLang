#include "parser.h"
#include "main.h"
#include "node.h"
#include "../lexer/token.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>


Node parse(const char *source, Parser *parser, Token *tokens) {
    parser = {
        tokens,
        source,
        .start = 0,
        .current = 0,
        .had_error = 0
    };

    Node *root = parse_program(parser);
}

// parsers

Node *parse_program(Parser *parser) {
    
}

Node parse_primary(Parser *parser, Token *token) {
    if (is_type(parser, TOKEN_NUMBER)) {
        return (Node) {
            .type = NODE_NUMBER_LITERAL,
            .position = get_position(*token),
            .as.number_literal = {
                .type = NUMBER_INT, // TODOL HANDLE FLOATS
                .int_value = read_token_int(parser->source, *token)
            }
        };
    } else if (is_type(parser, TOKEN_STRING)) {
        return (Node) {
            .type = NODE_STRING_LITERAL,
            .position = get_position(*token),
            .as.string_literal = {
                read_token(parser->source, *token)
            }
        };
    } else if (is_type(parser, TOKEN_IDENTIFIER)) {
        return (Node) {
            .type = NODE_IDENTIFIER,
            .position = get_position(*token),
            .as.identifier_expr = {
                read_token(parser->source, *token)
            }
        };
    }

    return ((Node) {});
}



// return the current token without consuming it
Token peek(Parser *parser) {
    return parser->tokens[parser->current];
}

// return the most recently consumed token
Token previous(Parser *parser) {
    return parser->tokens[parser->current - 1];
}

// are we at an eof token
int is_at_end(Parser *parser) {
    return peek(parser).type == TOKEN_EOF;
}

// consume the next token and return it
Token advance(Parser *parser) {
    if (!is_at_end(parser)) (parser->current)++;

    return previous(parser);
}

// is the current token this type
int is_type(Parser *parser, TokenType type) {
    return peek(parser).type == type;
}

// consume and return next token of a certain type. error if mismatched type.
Token consume(Parser *parser, TokenType type) {
    if (is_type(parser, type)) return advance(parser);

    // TODO: proper error handling
    printf("Expected token of type %-22s", token_names[type]);
    return (Token) {TOKEN_ERROR};
}

