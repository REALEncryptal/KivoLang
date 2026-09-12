#ifndef KIVO_NODE_H
#define KIVO_NODE_H

#include <stddef.h>
#include "../lexer/token.h"

typedef enum {

    // Base
    NODE_CHUNK, // files statements
    NODE_BLOCK, // statements inside a {...} block

    // Statements
    NODE_LET_STATEMENT, // let a = b
    NODE_ASSIGNMENT_STATEMENT, // a = b
    NODE_IF_STATEMENT,
    NODE_SWITCH_STATEMENT,
    NODE_WHILE_STATEMENT,
    NODE_REPEAT_STATEMENT,
    NODE_FOR_STATEMENT,
    NODE_RETURN_STATEMENT,
    NODE_BREAK_STATEMENT,
    NODE_CONTINUE_STATEMENT,

    // Expressions
    NODE_IDENTIFIER,
    NODE_NUMBER_LITERAL,
    NODE_STRING_LITERAL,
    NODE_BOOLEAN_LITERAL,
    
    NODE_UNARY_EXPRESSION, // !expr
    NODE_BINARY_EXPRESSION, // expr * expr
    NODE_CALL_EXPRESSION, // myFunc(args)
    NODE_MEMBER_EXPRESSION, // person.name
    NODE_INDEX_EXPRESSION, // persons["alex"]
    NODE_TABLE_EXPRESSION, // list of table entries
    NODE_FUNCTION_EXPRESSION, 
    NODE_IF_EXPRESSION,
    NODE_CLASS_EXPRESSION

} NodeType;


typedef struct SwitchCase SwitchCase;
typedef struct Node Node;

struct Node {
    NodeType type;
    size_t start;
    size_t length;

    union {
        // chunk
        struct {
            Node **statements; // need to make a growing list before anything else
        } chunk;

        // block
        struct {
            Node **statements; // need to make a growing list before anything else
        } block;

        // let 
        struct {
            Node *identifier;
            Node *expression;
        } let_stmnt;

        // assignment
        struct {
            Node *identifier;
            Node *expression;
        } assign_stmnt;

        // if 
        struct {
            Node *condition;
            Node *then_branch;
            Node *else_branch;
        } if_stmnt;

        // switch 
        struct {
            Node *expression;
            SwitchCase *cases;
            size_t case_count;
            Node *default_branch;
        } switch_stmnt;

        // while
        struct {
            Node *condition;
            Node *body; // NODE_BLOCK
        } while_stmnt;

        // repeat
        struct {
            Node *repeat_condition;
            Node *body; // NODE_BLOCK
        } repeat_stmnt;
    } as;
};

struct SwitchCase {
    Node *match;
    Node *body;
};

#endif // KIVO_NODE_H
