#ifndef SPOONER_TOKENS_H
#define SPOONER_TOKENS_H

#include <string.h>

#include "memory.h"

enum {
   TOKEN_EOF, TOKEN_NUMERIC, TOKEN_CHAR, TOKEN_STRING, TOKEN_OPERATOR,
   TOKEN_NAME, TOKEN_FUNCTION_CALL, TOKEN_LEFT_PARENS, TOKEN_RIGHT_PARENS, 
   TOKEN_LEFT_SQUARE_BRACKET, TOKEN_RIGHT_SQUARE_BRACKET
};

typedef struct {
   char *value;
   int type;
   int arity;
   int start_pos; /* where in the stream this token starts */
   int end_pos;
} Token;

Token *create_token(char *value, int type, int arity, int pos) {
   Token *token = (Token *)sp_malloc(sizeof(Token));
   *token = (Token){ value, type, arity, pos, (value ? pos + strlen(value) : pos) };
   return token;
}

Token *create_unary_token(char *value, int type, int pos) {
   return create_token(value, type, 0, pos);
}

Token *create_operator_token(char *op_value, int pos) {
   return create_token(op_value, TOKEN_OPERATOR, 2, pos);
}

Token *create_eof_token() {
   /* we don't really care about the position of this token, so set it to 0 */
   return create_token(NULL, TOKEN_EOF, 0, 0);
}

void free_token(Token *token) {
   sp_free(token->value);
   sp_free(token);
}

#endif
