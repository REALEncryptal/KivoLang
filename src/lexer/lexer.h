#ifndef KIVO_LEXER_H
#define KIVO_LEXER_H

#include "token.h"
#include <stddef.h>

Token* tokenize(char *source);
Token scan_number(char *source, size_t *current);
Token scan_string(char *source, size_t *current);
Token scan_comparator(char *source, size_t *current);
Token scan_identifier_and_keyword(char *source, size_t *current);
Token scan_comparison_and_logic(char *source, size_t *current);
int consume_match(char expected, char *source, size_t *current);

void print_token(Token *token, char *source);
void testing(void);

#endif // KIVO_LEXER_H
