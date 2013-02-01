#ifndef TOKENS_H
#define TOKENS_H

enum {
   TOKEN_EOF, TOKEN_NUMERIC, TOKEN_CHAR, TOKEN_STRING, TOKEN_OPERATOR,
   TOKEN_NAME, TOKEN_FUNCTION_CALL, TOKEN_LEFT_PARENS, TOKEN_RIGHT_PARENS, 
   TOKEN_LEFT_SQUARE_BRACKET, TOKEN_RIGHT_SQUARE_BRACKET
};

typedef struct {
   char *value;
   int type;
   int stream_pos; /* where in the stream this token starts */
   int arity;
} Token;

Token *create_eof_token(Token *token, int stream_pos) {
   *token = (Token){ NULL, TOKEN_EOF, stream_pos, 0 };
   return token;
}

#endif
