#include "parser.h"
#include "main.h"
#include "node.h"
#include "../lexer/token.h"
#include "source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Node parse(const char *source, Parser *parser, Token *tokens) {
    parser->tokens = tokens;
    parser->source = source;
    parser->current = 0;
    parser->start = 0;

    int err = parse_program(parser);

    return parser->root;
}

// parsers

int parse_program(Parser *parser) {
    int success = 1;

    parser->root = (Node) {
        NODE_CHUNK,
        (SourcePosition) {0, strlen(parser->source)},
        .as.chunk = {
            calloc(1000, (size_t)sizeof(Node)),// TODO: make a growin lsit
            0
        }
    };

    skip_seperators(parser);

    while (!is_type(parser, TOKEN_EOF)) {
        size_t last_position = parser->current;

        // pasre statement
        Node statement;
        success =  parse_statement(parser, &statement);
        if (!success) break;
        append_root_statement(parser, statement);

        // make sure its not looping
        if (parser->current == last_position) {
            parser->error_message = "parser looped";
            success = 0;
            break;
        }

        // skip seperators again
        skip_seperators(parser);
    }

    return success;
}

int parse_statement(Parser *parser, Node *statement) {
    Token token = peek(parser);

    if (token.type == TOKEN_LET) {
        if (parse_let_statement(parser, statement)) return 1;
        
    } else if (token.type == TOKEN_IDENTIFIER) {
        if (peek_next(parser).type == TOKEN_ASSIGN) {
            // *statement = parse_assignment_statement(parser);
        }
    } else {
        if (parse_expression(parser, statement)) return 1;
    }

    parser->error_message = "unable to parse expression.";
    return 0;
}

int parse_let_statement(Parser *parser, Node *statement) {
    // let
    Token let_token;
    if (!expect(parser, &let_token, TOKEN_LET)) return 0;

    // name
    Token name_token;
    if (!expect(parser, &name_token, TOKEN_IDENTIFIER)) return 0;
    
    // =
    if (!expect(parser, NULL, TOKEN_ASSIGN)) return 0;

    Node value;
    if (!parse_expression(parser, &value)) return 0;

    *statement = (Node) {
        NODE_LET_STATEMENT,
        (SourcePosition) {let_token.start, let_token.start + value.position.length},
        .as.let_stmnt = {
            name_token,
            &value
        }
    };

    return 1;
}

int parse_expression(Parser *parser, Node *expression) {
    return parse_primary(parser, expression); // todo: implement properly
}

int parse_primary(Parser *parser, Node *primary) {
    Token token = peek(parser);
    
    if (is_type(parser, TOKEN_NUMBER)) {
        advance(parser);

        int value;
        if (!read_token_int(parser->source, token, &value)) {
            parser->error_message = "failed to read int token";
            return 0;
        }

        *primary = (Node) {
            .type = NODE_NUMBER_LITERAL,
            .position = get_position(token),
            .as.number_literal = {
                .type = NUMBER_INT, // TODOL HANDLE FLOATS
                .int_value = value
            }
        }; return 1;
    } else if (is_type(parser, TOKEN_STRING)) {
        advance(parser);

        char value;
        if (!read_token(parser->source, token, &value)) {
            parser->error_message = "failed to read string token";
            return 0;
        }

        *primary = (Node) {
            .type = NODE_STRING_LITERAL,
            .position = get_position(token),
            .as.string_literal = {
                &value
            }
        }; return 1;
    } else if (is_type(parser, TOKEN_IDENTIFIER)) {
        advance(parser);

        char value;
        if (!read_token(parser->source, token, &value)) {
            parser->error_message = "failed to read identifier token";
            return 0;
        }

        *primary = (Node) {
            .type = NODE_IDENTIFIER,
            .position = get_position(token),
            .as.identifier_expr = {
                &value
            }
        }; return 1;
    } else if (is_type(parser, TOKEN_PAREN_LEFT)) {
        advance(parser);
        skip_seperators(parser);

        Node expression;
        if (!parse_expression(parser, &expression)) return 0;

        skip_seperators(parser);
        if (!expect(parser, NULL, TOKEN_PAREN_RIGHT)) return 0;

        *primary = expression; 
    }
    
    parser->error_message = "failed to parse primary. token is not number, string, or identifier.";
    return 0;
}


// utilities

void append_root_statement(Parser *parser, Node node) {
    parser->root.as.chunk.statements[parser->root.as.chunk.statement_count] = &node;
    parser->root.as.chunk.statement_count ++;
}

// return the current token without consuming it
Token peek(Parser *parser) {
    return parser->tokens[parser->current];
}

// return the next token without consuming it
Token peek_next(Parser *parser) {
    if (is_type(parser, TOKEN_EOF)) return peek(parser);

    return parser->tokens[parser->current + 1];
}

// return the most recently consumed token
Token peek_previous(Parser *parser) {
    return parser->tokens[parser->current - 1];
}

// are we at an eof token
int is_at_end(Parser *parser) {
    return peek(parser).type == TOKEN_EOF;
}

// consume the next token and return it
Token advance(Parser *parser) {
    if (!is_at_end(parser)) (parser->current)++;

    return peek_previous(parser);
}

// is the current token this type
int is_type(Parser *parser, TokenType type) {
    return peek(parser).type == type;
}

// skips all seperators ( \n , ; )
Token skip_seperators(Parser *parser) {
    while (
        is_type(parser, TOKEN_NEWLINE) 
        || is_type(parser, TOKEN_SEMICOLON)
    ) {
        advance(parser);
    }

    return peek(parser);
}

// consume and return next token of a certain type. error if mismatched type.
Token consume(Parser *parser, TokenType type) {
    if (is_type(parser, type)) return advance(parser);

    // TODO: proper error handling
    printf("Expected token of type %-22s", token_names[type]);
    return (Token) {TOKEN_ERROR};
}

int expect(Parser *parser, Token *result, TokenType type) {
    if (peek_next(parser).type == type) {
        if (result != NULL) *result = advance(parser);
        return 1;
    };

    parser->error_message = "expected token of type xxx, got xxx"; // TODO: fix
    return 0;  
}