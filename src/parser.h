#pragma once
#ifndef _LLP_PARSER_H_
#define _LLP_PARSER_H_

#include "tokenizer.h"
#include "ast.h"
#include "ring.h"

struct AST *parse_num(struct ring_token **tokens);
struct AST *parse_neg(struct ring_token **tokens);
struct AST *parse_divmul(struct ring_token **tokens);
struct AST *parse_subadd(struct ring_token **tokens);
struct AST *parse_ast(struct ring_token **tokens);

#endif
