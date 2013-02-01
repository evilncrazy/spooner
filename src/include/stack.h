#ifndef STACK_H
#define STACK_H

#define MAX_TOKEN_STACK 100

#include "tokens.h"

typedef struct {
   int count;
   Token tokens[MAX_TOKEN_STACK];
} TokenStack;

void stack_push(TokenStack *st, Token token) { st->tokens[st->count++] = token; }
Token *stack_top(TokenStack *st) { return &st->tokens[st->count - 1]; }
Token stack_pop(TokenStack *st) { return st->tokens[--st->count]; }
int stack_count(TokenStack *st) { return st->count; }

#endif
