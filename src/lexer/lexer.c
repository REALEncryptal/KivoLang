#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "lexer.h"
#include <ctype.h>

//TODO: Make sure all scanner loops stop at end of file

// example
/*

let my_var = "67 burgers"
let myVar2 = 12589

[TOKEN_LET<let>, TOKEN_IDENTIFIER<my_var>, TOKEN_ASSIGN<=>, TOKEN_STRING<"67 BURGERS">]
[TOKEN_LET<let>, TOKEN_IDENTIFIER<myVar2>, TOKEN_ASSIGN<=>, TOKEN_NUMBER<12589>]

*/

Token* tokenize(char *source) {
    Token *tokens = malloc(sizeof(Token) * 1000); // todo: dynamic list
    size_t start = 0;
    size_t current = 0; // track both
    size_t source_length = strlen(source);

    size_t token_count = 0; // for adding tokens

    // continue until we are done with the file
    while (1) {
        if (current >= source_length) break;
        start = current;
        char *c = source + current;

        Token scanned_token = (Token){ TOKEN_NONE };

        // scan
        if (isdigit(*c)) { // scan number
            scanned_token = scan_number(source, &current);
        } else if (isalpha(*c)) { //  scan alpha
            scanned_token = scan_identifier_and_keyword(source, &current);
        } else if (*c=='"') { // scan string
            scanned_token = scan_string(source, &current);
        } else if (!isspace(*c)) { // first char is a symbol
            TokenType type = TOKEN_NONE;

            switch (*c) {
                case '+': type = TOKEN_PLUS; break;
                case '-': type = TOKEN_MINUS; break;
                case '*': type = TOKEN_STAR; break;
                case '/': type = TOKEN_SLASH; break;
                case '^': type = TOKEN_CARET; break;
                case '%': type = TOKEN_PERCENT; break;

                case '[': type = TOKEN_BRACKET_LEFT; break;
                case ']': type = TOKEN_BRACKET_RIGHT; break;
                case '{': type = TOKEN_BRACE_LEFT; break;
                case '}': type = TOKEN_BRACE_RIGHT; break;
                case '(': type = TOKEN_PAREN_LEFT; break;
                case ')': type = TOKEN_PAREN_RIGHT; break;

                default: scanned_token = scan_comparison_and_logic(source, &current); break;
            }

            // only create the new token if we dont already have one set.
            if (scanned_token.type == TOKEN_NONE) {
                scanned_token = (Token) {type, current, 1};
            }
        }

        // add token
        if (scanned_token.type != TOKEN_NONE) {
            tokens[token_count] = scanned_token;
            token_count ++;
        }

        current++;
    }

    // add final eof token
    tokens[token_count] = (Token) {TOKEN_EOF};

    return tokens;
}

// SCANNERS
Token scan_number(char *source, size_t *current) {
    size_t start = *current;

    while (1) {
        char *c = source + *current;
        if ( !isdigit(*c) ) break; // not digit anymore, break out
        (*current)++;
    }

    return (Token) {
        TOKEN_NUMBER,
        start,
        *current - start
    };
}

Token scan_string(char *source, size_t *current) {
    (*current)++; // consume first "
    size_t start = *current; 

    while (1) {
        char *c = source + *current;
        if ( *c=='"' ) break; // not string end.
        (*current)++;
    }

    return (Token) {
        TOKEN_STRING,
        start,
        *current - start
    };
}

Token scan_identifier_and_keyword(char *source, size_t *current) {
    TokenType type = TOKEN_IDENTIFIER;
    size_t start = *current;

    while (1) {
        char *c = source + *current;
        if (
            !isalnum(*c) // is it a alnum?
            && !( *c == '_' ) // or a _?
        ) break; // we dont check if the first char is a digit because that is already done earlier
        (*current)++;
    }

    // check if its a reserved word
    size_t length = *current - start;
    char *text = source + start;

    if (length == 3 && strncmp(text, "let", length) == 0) {
        type = TOKEN_LET;
    } else if (length == 6 && strncmp(text, "global", length) == 0) {
        type = TOKEN_GLOBAL;
    } else if (length == 2 && strncmp(text, "if", length) == 0) {
        type = TOKEN_IF;
    } else if (length == 4 && strncmp(text, "else", length) == 0) {
        type = TOKEN_ELSE;
    } else if (length == 5 && strncmp(text, "while", length) == 0) {
        type = TOKEN_WHILE;
    } else if (length == 3 && strncmp(text, "for", length) == 0) {
        type = TOKEN_FOR;
    }

    return (Token) {
        type,
        start,
        *current - start
    };
}

Token scan_comparison_and_logic(char *source, size_t *current) {
    size_t start = *current;

    TokenType type;
    char *c = source + *current;
    char *next_char = source + *current + 1;

    switch (*c) {
        case ('='): type = consume_match('=', source, current) ? TOKEN_EQUAL : TOKEN_ASSIGN; break;
        case ('!'): type = consume_match('=', source, current) ? TOKEN_NOT_EQUAL : TOKEN_NOT; break;
        case ('<'): type = consume_match('=', source, current) ? TOKEN_LESS_EQUAL : TOKEN_LESS; break;
        case ('>'): type = consume_match('=', source, current) ? TOKEN_GREATER_EQUAL : TOKEN_GREATER; break;
        case ('&'): type = consume_match('&', source, current) ? TOKEN_AND : TOKEN_AMPERSAND; break;
        case ('|'): type = consume_match('|', source, current) ? TOKEN_OR : TOKEN_PIPE; break;
        default: printf("How did we get here? %c\n", *(next_char-1));
    }

    return (Token) {    
        type,
        start,
        *current + 1 - start
    };
}


// check if the next char == expected. if so consume and incrememnt current and return true
int consume_match(char expected, char *source, size_t *current) {
    if (*current >= strlen(source)) return 0;

    char *c = source + *current + 1;

    // matches
    if (*c == expected) {
        (*current)++; // consume
        return 1;
    }

    // doesnt match
    return 0;
}

// debugging

void print_token(Token *token, char *source) {
    printf("<%.*s>%s\n", token->length, source + token->start, token_names[token->type]);
}

void testing(void) {
    char source[] = "myVar 123 \"hello\" + - * / ^ % [ ] { } ( ) \"comparison!!!!\" = == ! != < <= > >= & && | || let global if else while \n";
    printf("Input: %s", source);

    Token *tokens = tokenize(source);

    // loop until eof   
    printf("Tokens:\n");
    for (int i=0; tokens[i].type != TOKEN_EOF; i++) {
        print_token(&tokens[i], source);
    }
    printf("---- EOF ----\n");
}
