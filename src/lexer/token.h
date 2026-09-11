#ifndef KIVO_TOKEN_H
#define KIVO_TOKEN_H

typedef enum {
    // Values
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER, 
    TOKEN_STRING,

    // Ops
    TOKEN_PLUS,    // +
    TOKEN_MINUS,   // -
    TOKEN_STAR,    // *
    TOKEN_SLASH,   // /
    TOKEN_CARET,   // ^
    TOKEN_PERCENT, // %

    // Grouping
    TOKEN_BRACKET_LEFT,  // [
    TOKEN_BRACKET_RIGHT, // ]
    TOKEN_BRACE_LEFT,    // {
    TOKEN_BRACE_RIGHT,   // }
    TOKEN_PAREN_LEFT,    // (
    TOKEN_PAREN_RIGHT,   // )

    // Assignment and comparison
    TOKEN_ASSIGN,        // =
    TOKEN_EQUAL,         // ==
    TOKEN_NOT,           // !
    TOKEN_NOT_EQUAL,     // !=
    TOKEN_LESS,          // <
    TOKEN_LESS_EQUAL,    // <=
    TOKEN_GREATER,       // >
    TOKEN_GREATER_EQUAL, // >=

    // Logic
    TOKEN_AMPERSAND, // &
    TOKEN_AND,       // &&
    TOKEN_PIPE,      // |
    TOKEN_OR,        // ||

    // Keywords
    TOKEN_LET,
    TOKEN_GLOBAL,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,

    // Special
    TOKEN_EOF, // end of file
    TOKEN_ERROR // inbavid text like some random chinese letter

} TokenType;

typedef struct {
    TokenType type;
    int start; // where does this token start (in source)
    int length; // start+length = end
} Token;

#endif // KIVO_TOKEN_H