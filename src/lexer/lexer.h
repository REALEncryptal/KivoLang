#ifndef KIVO_LEXER_H
#define KIVO_LEXER_H

#include "token.h"
#include <stddef.h>

Token* tokenize(const char *source);
Token scan_number(const char *source, size_t *current);
Token scan_string(const char *source, size_t *current);
Token scan_comparator(const char *source, size_t *current);
Token scan_identifier_and_keyword(const char *source, size_t *current);
Token scan_comparison_and_logic(const char *source, size_t *current);
int consume_match(char expected, const char *source, size_t *current);

void print_token(Token *token, const char *source);
void print_tokens(Token *tokens, const char *source);

#endif // KIVO_LEXER_H
