#ifndef KIVO_NODE_H
#define KIVO_NODE_H

#include <stddef.h>
#include "../lexer/token.h"
#include "../main.h"

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
    //NODE_INDEX_EXPRESSION, // persons["alex"] actually i dont like this, you should never use a index with a space
    NODE_TABLE_EXPRESSION, // list of table entries 
    NODE_FUNCTION_EXPRESSION, 
    NODE_IF_EXPRESSION,
    NODE_CLASS_EXPRESSION
    
} NodeType;


typedef struct SwitchCase SwitchCase;
typedef enum {
    NUMBER_INT,
    NUMBER_FLOAT,
} NumberLiteralType;
typedef struct Node Node;

struct Node {
    NodeType type;
    SourcePosition position;

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

        // for
        struct {
            Node *index_identifier;
            Node *value_identifier;
            Node *table;
            Node *body; // Node_BLOCK
        } for_stmnt;

        // return
        struct {
            Node *return_expression; // literally any expression as long as it becoems a value
        } return_stmnt;

        // continyue
        struct {} continue_stmnt;

        //== expressions
        // identifier
        struct {
            char *value;
        } identifier_expr;

        // number lit
        struct {
            NumberLiteralType type;
            union {
                int int_value;
                float float_value;
            };
        } number_literal;

        // string literal
        struct {
            char *value;
        } string_literal;

        // boolean literal
        struct {
            int value;
        } boolean_literal;

        //=== actual expressions fr
        // unary
        struct {
            TokenType operator;
            Node *operand;
        } unary_expr;

        // binary
        struct {
            TokenType operator;
            Node *left;
            Node *right;
        } binary_expr;

        // call functionName(123, "hello")
        struct {
            Node *identifier;
            Node *arguments; // pass in list of arguments
            int *argument_count;
        } call_expr;

        // Member person.name person.height
        struct {
            Node *parent;
            Node *identifier;
        } member_expr;

        // Table TODO: Implement table node struct
        struct {} table_expr;

        // Function.  let myfunc = (param){block}.  OR   let myfunc(param){block}
        struct {
            Node *identifier;
            Node *parameters; // list of idents
            Node *body; // BLOCK
        } function_expr;        

        // If EXPRESSION!   let result = if count > 1 then 100 else 3 * (100 * 2)
        struct {
            Node *condition;
            Node *truthy;
            Node *falsy;
        } if_expr;

        // Class TODO: Implement class node struct
        struct {} class_expr;
    } as;
};

struct SwitchCase {
    Node *match;
    Node *body;
};

#endif // KIVO_NODE_H
