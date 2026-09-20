# Parser Implementation Roadmap

This guide explains how to structure Kivo's recursive-descent parser, how
parsing begins, and where loops belong. It is intended as an implementation
roadmap rather than a complete implementation.

For a broader introduction to recursive descent, see
[Recursive Descent Parser Guide](recursive-descent-parser-guide.md).

## The central idea

Do not write one large loop that understands every possible token. Divide the
grammar into small parsing functions, where each function owns one grammar
rule:

```text
main
  -> tokenize(source)
  -> parse(source, tokens)
       -> parse_program()
            -> parse_statement()
                 -> parse_expression()
                      -> parse_primary()
```

The program parser owns the outer statement loop. Expression-precedence
functions own smaller loops for repeated operators.

## The cursor invariant

The parser's `current` field acts like a finger pointing at the next token to
read.

```text
LET  IDENTIFIER  ASSIGN  NUMBER  NEWLINE  EOF
 ^
 current
```

Every successful parsing function should obey this rule:

> Consume exactly the tokens belonging to your grammar rule, then leave
> `current` pointing at the first token you did not consume.

For example, given:

```text
123 + 4
```

after `parse_primary()` creates the node for `123`, `current` should point at
`+`.

This invariant lets parsing functions compose. A caller does not need to know
how its child parser works; it only needs to know where the child will stop.

## Decide AST ownership first

Kivo's AST contains child pointers such as `Node *left`, `Node *right`, and
`Node **statements`. Those pointers must continue to refer to valid objects
after a parsing function returns.

A straightforward ownership model for the first implementation is:

```text
- Parsing functions return Node*.
- Every successful node creation allocates a node with stable storage.
- A parent node owns its child nodes.
- The complete AST is eventually released by a recursive free_ast() function.
```

Conceptually, the parser declarations would have this shape:

```c
Node *parse_program(Parser *parser);
Node *parse_statement(Parser *parser);
Node *parse_expression(Parser *parser);
Node *parse_primary(Parser *parser);
```

This is not the only possible design. An arena allocator is another good
option later, but individually allocated nodes make the lifetime rules easier
to see while learning.

### Statement-list metadata

A chunk currently has a `Node **statements` pointer. A pointer alone does not
say how many statements are in the list. The chunk will need the conceptual
equivalent of:

```text
statements
statement_count
statement_capacity
```

The count describes how many entries are valid. The capacity describes how
many entries fit in the current allocation. When count reaches capacity, grow
the allocation before appending another statement.

## 1. The parsing entry point

The public `parse()` function should initialize the parser state and delegate
to the program parser. It should not contain all the grammar logic.

```text
function parse(source, tokens):
    parser.tokens = tokens
    parser.source = source
    parser.current = 0
    parser.had_error = false

    root = parse_program(&parser)

    if parser.had_error:
        decide whether to return a partial AST or a failure result

    return root
```

The complete front-end flow is then:

```text
source = read source file
tokens = tokenize(source)
ast = parse(source, tokens)

print or evaluate ast

free ast
free tokens
free source
```

Each allocation should have one clearly identified owner responsible for
freeing it.

## 2. The program loop

Start with a small grammar:

```text
program   -> separators* (statement separators*)* EOF
separator -> NEWLINE | SEMICOLON
```

The program parser owns the outer loop:

```text
function parse_program(parser):
    chunk = create empty chunk node

    skip_separators(parser)

    while not at EOF:
        position_before = parser.current

        statement = parse_statement(parser)

        if statement succeeded:
            append statement to chunk
        else:
            synchronize_to_next_statement(parser)

        # Protect against accidental infinite loops while developing.
        if parser.current == position_before:
            report "parser made no progress"
            advance(parser)

        skip_separators(parser)

    return chunk
```

The separator helper is a small loop:

```text
function skip_separators(parser):
    while current token is NEWLINE or SEMICOLON:
        advance(parser)
```

The progress check is especially useful during development. If a parser
reports success or failure without consuming anything, the outer loop would
otherwise examine the same token forever.

## 3. Statement dispatch

For the first milestone, statements can follow this grammar:

```text
statement           -> letStatement
                     | assignmentStatement
                     | expressionStatement

letStatement        -> LET IDENTIFIER ASSIGN expression
assignmentStatement -> IDENTIFIER ASSIGN expression
expressionStatement -> expression
```

`parse_statement()` should inspect the next token or two and delegate:

```text
function parse_statement(parser):
    if current token is LET:
        return parse_let_statement(parser)

    if current token is IDENTIFIER
       and next token is ASSIGN:
        return parse_assignment_statement(parser)

    return parse_expression_statement(parser)
```

This inspection is called **lookahead**. It chooses a grammar rule without
consuming any tokens.

Use one consistent convention:

> The dispatcher only inspects tokens. The delegated parser consumes its own
> starting token.

Under this convention, `parse_let_statement()` consumes `TOKEN_LET` itself.
`parse_statement()` does not consume it first.

### One-token lookahead

Assignment and expression statements may both begin with an identifier:

```text
x = 10       # assignment statement
x + 10       # expression statement
```

A non-consuming lookahead helper distinguishes them:

```text
function peek_next(parser):
    if current token is EOF:
        return EOF token

    return tokens[current + 1]
```

Be careful not to read beyond the token array. The lexer-provided EOF token is
the boundary marker.

## 4. Let statements

The grammar is:

```text
letStatement -> LET IDENTIFIER ASSIGN expression
```

Its parser can be reasoned about as follows:

```text
function parse_let_statement(parser):
    let_token = require LET
    name_token = require IDENTIFIER
    require ASSIGN

    value = parse_expression(parser)

    if any required part failed:
        return failure

    name = create identifier node from name_token

    return create let node:
        identifier = name
        expression = value
        position = from let_token through value
```

The let parser does not need to understand the inside of `2 + 3 * 4`. It
delegates that work to `parse_expression()`.

## 5. Primary expressions

Primary expressions are the smallest expression units:

```text
primary -> NUMBER
         | STRING
         | IDENTIFIER
         | LEFT_PAREN expression RIGHT_PAREN
```

The primary parser should consume whichever token it recognizes:

```text
function parse_primary(parser):
    if current token is NUMBER:
        token = advance(parser)
        return number node made from token

    if current token is STRING:
        token = advance(parser)
        return string node made from token

    if current token is IDENTIFIER:
        token = advance(parser)
        return identifier node made from token

    if current token is LEFT_PAREN:
        advance(parser)
        skip newlines

        expression = parse_expression(parser)

        skip newlines
        require RIGHT_PAREN

        return expression

    report "expected expression"
    return failure
```

The current Kivo primary parser recognizes values but does not advance the
cursor. If an outer loop called it, the same token would be parsed repeatedly.
The token used to build a node should be the exact token returned by
`advance()`.

Passing a separate token pointer to `parse_primary()` is unnecessary if the
function always parses the current token. Keeping the cursor and the token
argument separate also creates a risk that they refer to different tokens.

For the first working version, expression parsing can temporarily be:

```text
function parse_expression(parser):
    return parse_primary(parser)
```

This allows program structure, AST storage, and cursor movement to be tested
before operator precedence is added.

## 6. Expression precedence

Once primary expressions work, introduce precedence one level at a time:

```text
expression -> equality
equality   -> comparison ((EQUAL | NOT_EQUAL) comparison)*
comparison -> term ((LESS | LESS_EQUAL | GREATER | GREATER_EQUAL) term)*
term       -> factor ((PLUS | MINUS) factor)*
factor     -> unary ((STAR | SLASH | PERCENT) unary)*
unary      -> (NOT | MINUS) unary | primary
```

Each binary precedence level follows the same pattern. For addition and
subtraction:

```text
function parse_term(parser):
    left = parse_factor(parser)

    while current token is PLUS or MINUS:
        operator = advance(parser)
        right = parse_factor(parser)

        left = create binary node:
            operator = operator.type
            left = left
            right = right

    return left
```

The loop handles chains such as:

```text
1 + 2 - 3 + 4
```

The call to the next-lower level creates precedence. Because `parse_term()`
asks `parse_factor()` for each operand, this expression:

```text
2 + 3 * 4
```

produces this tree:

```text
    +
   / \
  2   *
     / \
    3   4
```

The tree means `2 + (3 * 4)`.

### Unary expressions

Unary parsing uses recursion rather than a binary loop:

```text
function parse_unary(parser):
    if current token is NOT or MINUS:
        operator = advance(parser)
        operand = parse_unary(parser)
        return unary(operator, operand)

    return parse_primary(parser)
```

Calling `parse_unary()` recursively allows sequences such as `!!value` or
`---5`.

## 7. Token helper responsibilities

It helps to give token helpers distinct meanings:

```text
peek()          inspect current token without consuming it
peek_next()     inspect the following token without consuming it
advance()       consume and return the current token
check(type)     test current token type without consuming it
match(type)     consume current token only if its type matches
require(type)   consume a matching token or report an error
```

Example contracts:

```text
function match(parser, expected):
    if not check(parser, expected):
        return false

    advance(parser)
    return true
```

```text
function require(parser, expected):
    if check(parser, expected):
        return advance(parser)

    report expected-token error at current token
    parser.had_error = true
    return failure token
```

When `require()` fails without advancing, its caller must stop parsing that
construct or explicitly recover. Continuing as though a token was consumed
can cause invalid nodes or an infinite loop.

Also remember that `previous()` is only valid after at least one successful
advance. Reading `tokens[current - 1]` while `current` is zero accesses memory
before the token array.

## 8. Error recovery

At first, it is acceptable to stop after the first syntax error. Once the
basic parser works, statement-level synchronization lets it report more than
one error in a file.

```text
function synchronize_to_next_statement(parser):
    while not at EOF:
        if current token is NEWLINE or SEMICOLON:
            advance(parser)
            return

        advance(parser)
```

This skips the remainder of the invalid statement and restarts at a likely
statement boundary.

The ownership rule during errors should be explicit too. If a parser allocates
part of a node and then fails, it must either:

- free the partial structure immediately, or
- transfer it into an error/partial AST that will later be freed normally.

Avoid losing pointers to partially constructed nodes.

## 9. Newline behavior

At the program level, newlines and semicolons separate statements. Ordinary
expression parsing should stop naturally when the current token is not an
operator belonging to the current precedence level.

Thus this is invalid under the current language design:

```kivo
let value = 1 +
    2
```

After `+`, the parser requires another operand, but encounters a newline.

Newlines can be skipped inside explicit grouping constructs, making this
valid:

```kivo
let value = (
    1 +
    2
)
```

The grouping parser controls this exception by skipping newlines after `(`
and before `)`.

## 10. Suggested implementation milestones

Work in small steps, and verify cursor movement after each one.

### Milestone 1: Cursor and primary expressions

- Make primary parsing consume recognized tokens.
- Decide how nodes are allocated and freed.
- Temporarily make `parse_expression()` call only `parse_primary()`.
- Verify number, string, and identifier nodes.

Suggested input:

```kivo
123
"hello"
some_name
```

Before running it, predict where `current` should point after each primary.

### Milestone 2: Program storage

- Add count and capacity information to statement storage.
- Implement append/grow behavior.
- Implement `parse_program()` and separator skipping.
- Verify that the chunk contains three nodes and parsing ends at EOF.

### Milestone 3: Grouping and unary operators

- Add parenthesized expressions.
- Add unary `!` and `-`.
- Test nested groups and repeated unary operators.

Suggested inputs:

```kivo
(123)
((123))
-123
!!some_name
```

### Milestone 4: Arithmetic precedence

- Add factor parsing for `*`, `/`, and `%`.
- Add term parsing for `+` and `-`.
- Print or inspect the resulting tree.

Suggested input:

```kivo
2 + 3 * 4
```

Expected structure:

```text
2 + (3 * 4)
```

### Milestone 5: Statements

- Add let statements.
- Add one-token lookahead.
- Add assignment statements.
- Decide whether expression statements need a dedicated AST node or whether
  expressions may appear directly in a chunk.

Suggested inputs:

```kivo
let result = 2 + 3 * 4
result = result + 1
result
```

### Milestone 6: Comparisons and later features

- Add comparison and equality precedence.
- Add calls and member access.
- Add blocks using a statement loop that stops at `}` instead of EOF.
- Add control flow only after expressions and blocks are reliable.

## A useful debugging trace

During early development, temporarily log function entry, current token, and
function exit. For example:

```text
enter parse_term       current=NUMBER "2"
  enter parse_factor   current=NUMBER "2"
  exit parse_factor    current=PLUS "+"
  consume PLUS
  enter parse_factor   current=NUMBER "3"
    consume STAR
  exit parse_factor    current=EOF
exit parse_term        current=EOF
```

This makes violations of the cursor invariant visible. If a successful
function enters and exits at the same token unexpectedly, it probably forgot
to consume something.

## Readiness checklist

Before adding blocks or control flow, confirm all of the following:

- [ ] Every successful primary expression advances the cursor.
- [ ] Every parse function documents which tokens it consumes.
- [ ] The program loop always makes progress or reaches EOF.
- [ ] Statement storage has both a count and a capacity.
- [ ] AST nodes have stable lifetimes after parsing functions return.
- [ ] Every allocated node has a clear owner.
- [ ] `2 + 3 * 4` produces multiplication beneath addition.
- [ ] Parentheses can override precedence.
- [ ] A missing required token sets the parser's error state.
- [ ] Error paths do not leak partially constructed nodes.
- [ ] Newline behavior matches the intended Kivo grammar.

Once these properties hold, the parser has a solid foundation for calls,
blocks, conditionals, and loops.
