#ifndef SPOONER_STACK_H
#define SPOONER_STACK_H

#include "limits.h"
#include "tokens.h"

/* stack for holding tokens and opcodes */
typedef struct {
   int count;
   Token *tokens[MAX_LINE_NUM_OPCODES];
} TokenStack;

void stack_push(TokenStack *st, Token *token) { st->tokens[st->count++] = token; }
Token *stack_top(TokenStack *st) { return st->tokens[st->count - 1]; }
Token *stack_pop(TokenStack *st) { return st->tokens[--st->count]; }
int stack_count(TokenStack *st) { return st->count; }

#endif
