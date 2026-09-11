#include <stdio.h>
#include <string.h>
#include "token.h"

// example
/*

let my_var = "67 burgers"
let myVar2 = 12589

[TOKEN_LET<let>, TOKEN_IDENTIFIER<my_var>, TOKEN_ASSIGN<=>, TOKEN_STRING<"67 BURGERS">]
[TOKEN_LET<let>, TOKEN_IDENTIFIER<myVar2>, TOKEN_ASSIGN<=>, TOKEN_NUMBER<12589>]

*/

Token* tokenize(char *source) {
    Token tokens[1000] = {}; // Todo: dyamic list
    size_t start, current = 0; // track both
    size_t source_length = strlen(source);

    size_t token_count = 0; // for adding tokens

    // continue until we are done with the file
    while (start < source_length) {
        start = current;

        
    }
}



void testing() {
    char source[] = "let * + - testing 123 \"hello\" ";
    printf("Input: %s", source);

    Token *tokens = tokenize(source);
}