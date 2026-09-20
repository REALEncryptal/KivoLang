#include "lexer/lexer.h"
#include "parser/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define CHECK(condition)                                                        \
    do {                                                                        \
        if (!(condition)) {                                                     \
            fprintf(stderr, "%s:%d: check failed: %s\n",                      \
                    __FILE__, __LINE__, #condition);                            \
            failures++;                                                         \
        }                                                                       \
    } while (0)

typedef struct {
    Parser parser;
    Token *tokens;
} ParserFixture;

static ParserFixture make_parser(const char *source) {
    Token *tokens = tokenize(source);
    Parser parser = {0};

    parser.tokens = tokens;
    parser.source = source;

    return (ParserFixture){parser, tokens};
}

static void destroy_parser(ParserFixture *fixture) {
    free(fixture->tokens);
    fixture->tokens = NULL;
}

static void destroy_primary(Node *node) {
    if (node->type == NODE_STRING_LITERAL) {
        free(node->as.string_literal.value);
        node->as.string_literal.value = NULL;
    } else if (node->type == NODE_IDENTIFIER) {
        free(node->as.identifier_expr.value);
        node->as.identifier_expr.value = NULL;
    }
}

static void test_cursor_helpers_inspect_and_consume_tokens(void) {
    ParserFixture fixture = make_parser("12 name");
    Parser *parser = &fixture.parser;

    CHECK(peek(parser).type == TOKEN_NUMBER);
    CHECK(peek_next(parser).type == TOKEN_IDENTIFIER);
    CHECK(parser->current == 0);

    Token number = advance(parser);
    CHECK(number.type == TOKEN_NUMBER);
    CHECK(number.start == 0);
    CHECK(number.length == 2);
    CHECK(parser->current == 1);
    CHECK(peek_previous(parser).type == TOKEN_NUMBER);

    Token identifier = consume(parser, TOKEN_IDENTIFIER);
    CHECK(identifier.type == TOKEN_IDENTIFIER);
    CHECK(parser->current == 2);
    CHECK(is_at_end(parser));

    destroy_parser(&fixture);
}

static void test_peek_next_at_eof_returns_the_eof_token(void) {
    ParserFixture fixture = make_parser("");
    Parser *parser = &fixture.parser;

    Token next = peek_next(parser);

    CHECK(next.type == TOKEN_EOF);
    CHECK(parser->current == 0);
    CHECK(is_at_end(parser));

    destroy_parser(&fixture);
}

static void test_skip_separators_stops_at_first_non_separator(void) {
    ParserFixture fixture = make_parser("\n;\n123");
    Parser *parser = &fixture.parser;

    Token next = skip_seperators(parser);

    CHECK(next.type == TOKEN_NUMBER);
    CHECK(parser->current == 3);
    CHECK(peek(parser).start == 3);

    destroy_parser(&fixture);
}

static void test_expect_consumes_the_current_matching_token(void) {
    ParserFixture fixture = make_parser("123");
    Parser *parser = &fixture.parser;
    Token result = {0};

    int success = expect(parser, &result, TOKEN_NUMBER);

    CHECK(success);
    if (success) {
        CHECK(result.type == TOKEN_NUMBER);
        CHECK(result.start == 0);
        CHECK(result.length == 3);
    }
    CHECK(parser->current == 1);
    CHECK(is_at_end(parser));

    destroy_parser(&fixture);
}

static void test_expect_rejects_a_mismatch_without_advancing(void) {
    ParserFixture fixture = make_parser("name");
    Parser *parser = &fixture.parser;

    int success = expect(parser, NULL, TOKEN_NUMBER);

    CHECK(!success);
    CHECK(parser->current == 0);
    CHECK(peek(parser).type == TOKEN_IDENTIFIER);
    CHECK(parser->error_message != NULL);

    destroy_parser(&fixture);
}

static void test_parse_number_primary(void) {
    ParserFixture fixture = make_parser("123");
    Parser *parser = &fixture.parser;
    Node node = {0};

    int success = parse_primary(parser, &node);

    CHECK(success);
    if (success) {
        CHECK(node.type == NODE_NUMBER_LITERAL);
        CHECK(node.position.start == 0);
        CHECK(node.position.length == 3);
        CHECK(node.as.number_literal.type == NUMBER_INT);
        CHECK(node.as.number_literal.int_value == 123);
    }
    CHECK(is_at_end(parser));

    destroy_parser(&fixture);
}

static void test_parse_string_primary(void) {
    ParserFixture fixture = make_parser("\"hello\"");
    Parser *parser = &fixture.parser;
    Node node = {0};

    int success = parse_primary(parser, &node);

    CHECK(success);
    if (success) {
        CHECK(node.type == NODE_STRING_LITERAL);
        CHECK(node.position.start == 1);
        CHECK(node.position.length == 5);
        CHECK(strcmp(node.as.string_literal.value, "hello") == 0);
        destroy_primary(&node);
    }
    CHECK(is_at_end(parser));

    destroy_parser(&fixture);
}

static void test_parse_identifier_primary(void) {
    ParserFixture fixture = make_parser("answer_2");
    Parser *parser = &fixture.parser;
    Node node = {0};

    int success = parse_primary(parser, &node);

    CHECK(success);
    if (success) {
        CHECK(node.type == NODE_IDENTIFIER);
        CHECK(node.position.start == 0);
        CHECK(node.position.length == 8);
        CHECK(strcmp(node.as.identifier_expr.value, "answer_2") == 0);
        destroy_primary(&node);
    }
    CHECK(is_at_end(parser));

    destroy_parser(&fixture);
}

static void test_parse_grouped_primary_allows_newlines(void) {
    ParserFixture fixture = make_parser("(\n123\n)");
    Parser *parser = &fixture.parser;
    Node node = {0};

    int success = parse_primary(parser, &node);

    CHECK(success);
    if (success) {
        CHECK(node.type == NODE_NUMBER_LITERAL);
        CHECK(node.as.number_literal.int_value == 123);
    }
    CHECK(is_at_end(parser));

    destroy_parser(&fixture);
}

static void test_parse_primary_rejects_an_operator(void) {
    ParserFixture fixture = make_parser("+");
    Parser *parser = &fixture.parser;
    Node node = {0};

    int success = parse_primary(parser, &node);

    CHECK(!success);
    CHECK(parser->current == 0);
    CHECK(parser->error_message != NULL);

    destroy_parser(&fixture);
}

static void test_parse_empty_program(void) {
    ParserFixture fixture = make_parser("\n;\n");
    Parser *parser = &fixture.parser;

    int success = parse_program(parser);

    CHECK(success);
    CHECK(parser->root.type == NODE_CHUNK);
    CHECK(parser->root.position.start == 0);
    CHECK(parser->root.position.length == 3);
    CHECK(parser->root.as.chunk.statement_count == 0);
    CHECK(is_at_end(parser));

    free(parser->root.as.chunk.statements);
    parser->root.as.chunk.statements = NULL;
    destroy_parser(&fixture);
}

static void test_parse_identifier_expression_statement(void) {
    ParserFixture fixture = make_parser("answer");
    Parser *parser = &fixture.parser;
    Node statement = {0};

    int success = parse_statement(parser, &statement);

    CHECK(success);
    if (success) {
        CHECK(statement.type == NODE_IDENTIFIER);
        CHECK(strcmp(statement.as.identifier_expr.value, "answer") == 0);
        destroy_primary(&statement);
    }
    CHECK(is_at_end(parser));

    destroy_parser(&fixture);
}

static void test_parse_program_collects_separated_statements(void) {
    ParserFixture fixture = make_parser("1\n2;3");
    Parser *parser = &fixture.parser;

    int success = parse_program(parser);

    CHECK(success);
    CHECK(is_at_end(parser));
    CHECK(parser->root.as.chunk.statement_count == 3);
    if (parser->root.as.chunk.statement_count == 3) {
        CHECK(parser->root.as.chunk.statements[0] != NULL);
        CHECK(parser->root.as.chunk.statements[1] != NULL);
        CHECK(parser->root.as.chunk.statements[2] != NULL);

        if (parser->root.as.chunk.statements[0] != NULL) {
            CHECK(parser->root.as.chunk.statements[0]->type ==
                  NODE_NUMBER_LITERAL);
            CHECK(parser->root.as.chunk.statements[0]
                      ->as.number_literal.int_value == 1);
        }
        if (parser->root.as.chunk.statements[1] != NULL) {
            CHECK(parser->root.as.chunk.statements[1]->type ==
                  NODE_NUMBER_LITERAL);
            CHECK(parser->root.as.chunk.statements[1]
                      ->as.number_literal.int_value == 2);
        }
        if (parser->root.as.chunk.statements[2] != NULL) {
            CHECK(parser->root.as.chunk.statements[2]->type ==
                  NODE_NUMBER_LITERAL);
            CHECK(parser->root.as.chunk.statements[2]
                      ->as.number_literal.int_value == 3);
        }
    }

    free(parser->root.as.chunk.statements);
    parser->root.as.chunk.statements = NULL;
    destroy_parser(&fixture);
}

static void test_parse_let_statement(void) {
    ParserFixture fixture = make_parser("let answer = 42");
    Parser *parser = &fixture.parser;
    Node statement = {0};

    int success = parse_let_statement(parser, &statement);

    CHECK(success);
    if (success) {
        CHECK(statement.type == NODE_LET_STATEMENT);
        CHECK(statement.position.start == 0);
        CHECK(statement.position.length == strlen("let answer = 42"));
        CHECK(statement.as.let_stmnt.identifier.type == TOKEN_IDENTIFIER);
        CHECK(statement.as.let_stmnt.identifier.start == 4);
        CHECK(statement.as.let_stmnt.identifier.length == 6);
        CHECK(statement.as.let_stmnt.expression != NULL);
        if (statement.as.let_stmnt.expression != NULL) {
            CHECK(statement.as.let_stmnt.expression->type ==
                  NODE_NUMBER_LITERAL);
            CHECK(statement.as.let_stmnt.expression
                      ->as.number_literal.int_value == 42);
        }
    }
    CHECK(is_at_end(parser));

    destroy_parser(&fixture);
}

int main(void) {
    test_cursor_helpers_inspect_and_consume_tokens();
    test_peek_next_at_eof_returns_the_eof_token();
    test_skip_separators_stops_at_first_non_separator();
    test_expect_consumes_the_current_matching_token();
    test_expect_rejects_a_mismatch_without_advancing();
    test_parse_number_primary();
    test_parse_string_primary();
    test_parse_identifier_primary();
    test_parse_grouped_primary_allows_newlines();
    test_parse_primary_rejects_an_operator();
    test_parse_empty_program();
    test_parse_identifier_expression_statement();
    test_parse_program_collects_separated_statements();
    test_parse_let_statement();

    if (failures != 0) {
        fprintf(stderr, "%d parser test(s) failed\n", failures);
        return EXIT_FAILURE;
    }

    puts("parser tests passed");
    return EXIT_SUCCESS;
}
