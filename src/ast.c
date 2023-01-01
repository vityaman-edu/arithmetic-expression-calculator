#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

#define ERROR(value, message)                                                  \
  fprintf(stderr, "Error at %s line %d: " message, __FILE__, __LINE__), value

struct AST *newnode(struct AST ast) {
  struct AST *const node = malloc(sizeof(struct AST));
  *node = ast;
  return node;
}

struct AST _lit(int64_t value) {
  return (struct AST){AST_LIT, .as_literal = {value}};
}

struct AST *lit(int64_t value) {
  return newnode(_lit(value));
}

struct AST _unop(enum unop_type type, struct AST *operand) {
  return (struct AST){AST_UNOP, .as_unop = {type, operand}};
}

struct AST *unop(enum unop_type type, struct AST *operand) {
  return newnode(_unop(type, operand));
}

struct AST _binop(enum binop_type type, struct AST *left, struct AST *right) {
  return (struct AST){AST_BINOP, .as_binop = {type, left, right}};
}

struct AST *binop(enum binop_type type, struct AST *left, struct AST *right) {
  return newnode(_binop(type, left, right));
}

static const char *BINOPS[] = {
    [BIN_PLUS] = "+", [BIN_MINUS] = "-", [BIN_MUL] = "*", [BIN_DIV] = "/"};
static const char *UNOPS[] = {[UN_NEG] = "-"};

typedef void(printer)(FILE *, struct AST *);

static void print_binop(FILE *f, struct AST *ast) {
  fprintf(f, "(");
  print_ast(f, ast->as_binop.left);
  fprintf(f, ")");
  fprintf(f, "%s", BINOPS[ast->as_binop.type]);
  fprintf(f, "(");
  print_ast(f, ast->as_binop.right);
  fprintf(f, ")");
}

static void print_unop(FILE *f, struct AST *ast) {
  fprintf(f, "%s(", UNOPS[ast->as_unop.type]);
  print_ast(f, ast->as_unop.operand);
  fprintf(f, ")");
}

static void print_lit(FILE *f, struct AST *ast) {
  fprintf(f, "%" PRId64, ast->as_literal.value);
}

static printer *ast_printers[] = {
    [AST_BINOP] = print_binop, [AST_UNOP] = print_unop, [AST_LIT] = print_lit};

void print_ast(FILE *f, struct AST *ast) {
  if (ast)
    ast_printers[ast->type](f, ast);
  else
    fprintf(f, "<NULL>");
}

int64_t calc_ast(struct AST *ast);

int64_t calc_binop(struct binop *binop) {
  enum binop_type type = binop->type;
  struct AST *left = binop->left;
  struct AST *right = binop->right;
  if (type == BIN_PLUS) {
    return calc_ast(left) + calc_ast(right);
  }
  if (type == BIN_MINUS) {
    return calc_ast(left) - calc_ast(right);
  }
  if (type == BIN_MUL) {
    return calc_ast(left) * calc_ast(right);
  }
  if (type == BIN_DIV) {
    return calc_ast(left) / calc_ast(right);
  }
  return ERROR(66666, "unsupported binop type");
}

int64_t calc_unop(struct unop *unop) {
  enum unop_type type = unop->type;
  struct AST *operand = unop->operand;
  if (type == UN_NEG) {
    return -calc_ast(operand);
  }
  return ERROR(66666, "unsupported unop type");
}

int64_t calc_literal(struct literal *literal) { return literal->value; }

int64_t calc_ast(struct AST *ast) {
  enum AST_type type = ast->type;
  if (type == AST_BINOP) {
    return calc_binop(&ast->as_binop);
  }
  if (type == AST_UNOP) {
    return calc_unop(&ast->as_unop);
  }
  if (type == AST_LIT) {
    return calc_literal(&ast->as_literal);
  }
  return ERROR(66666, "unsupported AST type");
}

void p_print_ast(FILE *f, struct AST *ast);

void p_print_binop(FILE *f, struct AST *ast) {
  fprintf(f, "%s", BINOPS[ast->as_binop.type]);
  fprintf(f, "(");
  p_print_ast(f, ast->as_binop.left);
  fprintf(f, ")");
  fprintf(f, "(");
  p_print_ast(f, ast->as_binop.right);
  fprintf(f, ")");
}

void p_print_unop(FILE *f, struct AST *ast) { print_unop(f, ast); }

void p_print_lit(FILE *f, struct AST *ast) { print_lit(f, ast); }

static printer *ast_p_printers[] = {[AST_BINOP] = p_print_binop,
                                    [AST_UNOP] = p_print_unop,
                                    [AST_LIT] = p_print_lit};

void p_print_ast(FILE *f, struct AST *ast) {
  if (ast)
    ast_p_printers[ast->type](f, ast);
  else
    fprintf(f, "<NULL>");
}
