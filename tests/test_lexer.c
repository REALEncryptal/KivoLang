#include "lexer/lexer.h"

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
    TokenType type;
    int start;
    int length;
} ExpectedToken;

static const char *safe_token_name(TokenType type) {
    if ((int)type < 0 || type > TOKEN_NONE) return "<invalid token type>";
    return token_names[type];
}

static void check_tokens(const char *source,
                         const ExpectedToken *expected,
                         size_t expected_count) {
    Token *tokens = tokenize(source);

    CHECK(tokens != NULL);
    if (tokens == NULL) return;

    for (size_t i = 0; i < expected_count; i++) {
        if (tokens[i].type != expected[i].type ||
            tokens[i].start != expected[i].start ||
            tokens[i].length != expected[i].length) {
            fprintf(stderr,
                    "token %zu: expected {%s, %d, %d}, got {%s, %d, %d}\n",
                    i,
                    safe_token_name(expected[i].type),
                    expected[i].start,
                    expected[i].length,
                    safe_token_name(tokens[i].type),
                    tokens[i].start,
                    tokens[i].length);
            failures++;
        }
    }

    CHECK(tokens[expected_count].type == TOKEN_EOF);
    free(tokens);
}

static void test_empty_source_and_whitespace(void) {
    check_tokens("", NULL, 0);

    const ExpectedToken expected[] = {
        {TOKEN_NEWLINE, 3, 1},
    };
    check_tokens(" \t\r\n  ", expected, sizeof(expected) / sizeof(expected[0]));
}

static void test_values_and_token_spans(void) {
    const char *source = "name_2 123 \"hello world\" \"\"";
    const ExpectedToken expected[] = {
        {TOKEN_IDENTIFIER, 0, 6},
        {TOKEN_NUMBER, 7, 3},
        {TOKEN_STRING, 12, 11},
        {TOKEN_STRING, 26, 0},
    };

    check_tokens(source, expected, sizeof(expected) / sizeof(expected[0]));
}

static void test_keywords_and_keyword_prefixes(void) {
    const char *source = "let global if else while for letter global_ if2";
    const ExpectedToken expected[] = {
        {TOKEN_LET, 0, 3},
        {TOKEN_GLOBAL, 4, 6},
        {TOKEN_IF, 11, 2},
        {TOKEN_ELSE, 14, 4},
        {TOKEN_WHILE, 19, 5},
        {TOKEN_FOR, 25, 3},
        {TOKEN_IDENTIFIER, 29, 6},
        {TOKEN_IDENTIFIER, 36, 7},
        {TOKEN_IDENTIFIER, 44, 3},
    };

    check_tokens(source, expected, sizeof(expected) / sizeof(expected[0]));
}

static void test_arithmetic_grouping_and_punctuation(void) {
    const char *source = "+-*/^%[]{}().,;\n";
    const TokenType types[] = {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH, TOKEN_CARET,
        TOKEN_PERCENT, TOKEN_BRACKET_LEFT, TOKEN_BRACKET_RIGHT,
        TOKEN_BRACE_LEFT, TOKEN_BRACE_RIGHT, TOKEN_PAREN_LEFT,
        TOKEN_PAREN_RIGHT, TOKEN_DOT, TOKEN_COMMA, TOKEN_SEMICOLON,
        TOKEN_NEWLINE,
    };
    ExpectedToken expected[sizeof(types) / sizeof(types[0])];

    for (size_t i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
        expected[i] = (ExpectedToken){types[i], (int)i, 1};
    }

    check_tokens(source, expected, sizeof(expected) / sizeof(expected[0]));
}

static void test_comparison_and_logic_operators(void) {
    const char *source = "= == ! != < <= > >= & && | ||";
    const ExpectedToken expected[] = {
        {TOKEN_ASSIGN, 0, 1},
        {TOKEN_EQUAL, 2, 2},
        {TOKEN_NOT, 5, 1},
        {TOKEN_NOT_EQUAL, 7, 2},
        {TOKEN_LESS, 10, 1},
        {TOKEN_LESS_EQUAL, 12, 2},
        {TOKEN_GREATER, 15, 1},
        {TOKEN_GREATER_EQUAL, 17, 2},
        {TOKEN_AMPERSAND, 20, 1},
        {TOKEN_AND, 22, 2},
        {TOKEN_PIPE, 25, 1},
        {TOKEN_OR, 27, 2},
    };

    check_tokens(source, expected, sizeof(expected) / sizeof(expected[0]));
}

static void test_numbers_stop_at_a_decimal_point(void) {
    const ExpectedToken expected[] = {
        {TOKEN_NUMBER, 0, 2},
        {TOKEN_DOT, 2, 1},
        {TOKEN_NUMBER, 3, 2},
    };

    check_tokens("12.34", expected, sizeof(expected) / sizeof(expected[0]));
}

static void test_invalid_input(void) {
    const ExpectedToken invalid[] = {
        {TOKEN_ERROR, 0, 1},
    };

    check_tokens("@", invalid, sizeof(invalid) / sizeof(invalid[0]));
}

static void test_consume_match(void) {
    size_t current = 0;
    CHECK(consume_match('=', "==", &current) == 1);
    CHECK(current == 1);

    current = 0;
    CHECK(consume_match('=', "=", &current) == 0);
    CHECK(current == 0);

    current = 0;
    CHECK(consume_match('=', "!=", &current) == 1);
    CHECK(current == 1);
}

int main(void) {
    test_empty_source_and_whitespace();
    test_values_and_token_spans();
    test_keywords_and_keyword_prefixes();
    test_arithmetic_grouping_and_punctuation();
    test_comparison_and_logic_operators();
    test_numbers_stop_at_a_decimal_point();
    test_invalid_input();
    test_consume_match();

    if (failures != 0) {
        fprintf(stderr, "%d lexer test(s) failed\n", failures);
        return EXIT_FAILURE;
    }

    puts("lexer tests passed");
    return EXIT_SUCCESS;
}
