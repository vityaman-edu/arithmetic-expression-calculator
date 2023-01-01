#include "parser.h"
#include "tokenizer.h"
#include <stdbool.h>

#define UNEXPECTED_TOKEN(ret, actual, expectation)                             \
  fprintf(stderr,                                                              \
          "Error at %s line %d: got %s(%" PRId64 "), expected " expectation    \
          "\n",                                                                \
          __FILE__, __LINE__, TOKENS_STR[actual.type], actual.value),          \
      ret

#define EXPECT(expected_type, tokens)                                          \
  {                                                                            \
    struct token tok = peek_token(tokens);                                     \
    if (tok.type != expected_type) {                                           \
      return NULL;                                                             \
    }                                                                          \
  }

#define TAKE_EXPECTED(expected_type, varname, expectation, tokens)             \
  EXPECT(expected_type, tokens)                                                \
  struct token varname = ring_token_pop_top(tokens)

#define SKIP_EXPECTED(expected_type, expectation, tokens)                      \
  EXPECT(expected_type, tokens)                                                \
  ring_token_pop_top(tokens)

#define TRY_OR_FAIL(parser, varname, args...)                                  \
  struct AST *varname = parser(args);                                          \
  if (!varname)                                                                \
    return NULL;

#define TRY_OR_CONTINUE(parser, args...)                                       \
  {                                                                            \
    struct AST *ast = parser(args);                                            \
    if (ast) {                                                                 \
      return ast;                                                              \
    }                                                                          \
  }

DEFINE_RING(token, struct token);


struct token peek_token(struct ring_token **tokens) {
  if (*tokens == NULL) {
    return (struct token){.type = TOK_ERROR};
  }
  return ring_token_first(*tokens);
}

struct AST *parse_num(struct ring_token **tokens) {
  TAKE_EXPECTED(TOK_LIT, num, "number ('1'..'9')", tokens);
  return lit(num.value);
}

struct AST *parse_neg_alt_neg(struct ring_token **tokens) {
  SKIP_EXPECTED(TOK_NEG, "neg sign '-'", tokens);
  TRY_OR_FAIL(parse_neg, subexrp, tokens);
  return neg(subexrp);
}

struct AST *parse_neg_alt_brackets(struct ring_token **tokens) {
  SKIP_EXPECTED(TOK_OPEN, "openning bracket '('", tokens);
  TRY_OR_FAIL(parse_ast, expr, tokens);
  SKIP_EXPECTED(TOK_CLOSE, "closing bracket ')'", tokens);
  return expr;
}

struct AST *parse_neg_alt_num(struct ring_token **tokens) {
  return parse_num(tokens);
}

struct AST *parse_neg(struct ring_token **tokens) {
  TRY_OR_CONTINUE(parse_neg_alt_neg, tokens);
  TRY_OR_CONTINUE(parse_neg_alt_brackets, tokens);
  TRY_OR_CONTINUE(parse_neg_alt_num, tokens);
  return NULL;
}

struct AST *parse_divmul_with_tail(struct AST *left,
                                   struct ring_token **tokens) {
  struct token next = peek_token(tokens);
  if (next.type == TOK_MUL) {
    SKIP_EXPECTED(TOK_MUL, "mul '*'", tokens);
    TRY_OR_FAIL(parse_neg, right, tokens);
    TRY_OR_CONTINUE(parse_divmul_with_tail, mul(left, right), tokens);
    return NULL;
  }
  if (next.type == TOK_DIV) {
    SKIP_EXPECTED(TOK_DIV, "div '/'", tokens);
    TRY_OR_FAIL(parse_neg, right, tokens);
    TRY_OR_CONTINUE(parse_divmul_with_tail, divide(left, right), tokens);
    return NULL;
  }
  return left;
}

struct AST *parse_divmul(struct ring_token **tokens) {
  TRY_OR_FAIL(parse_neg, left, tokens);
  return parse_divmul_with_tail(left, tokens);
}

struct AST *parse_subadd_with_tail(struct AST *left,
                                   struct ring_token **tokens) {
  struct token next = peek_token(tokens);
  if (next.type == TOK_PLUS) {
    SKIP_EXPECTED(TOK_PLUS, "plus '+'", tokens);
    TRY_OR_FAIL(parse_divmul, right, tokens);
    TRY_OR_CONTINUE(parse_subadd_with_tail, add(left, right), tokens);
    return NULL;
  }
  if (next.type == TOK_MINUS) {
    SKIP_EXPECTED(TOK_MINUS, "minus '-'", tokens);
    TRY_OR_FAIL(parse_divmul, right, tokens);
    TRY_OR_CONTINUE(parse_subadd_with_tail, sub(left, right), tokens);
    return NULL;
  }
  return left;
}

struct AST *parse_subadd(struct ring_token **tokens) {
  TRY_OR_FAIL(parse_divmul, left, tokens);
  return parse_subadd_with_tail(left, tokens);
}

struct AST *parse_ast(struct ring_token **tokens) {
  return parse_subadd(tokens);
}
