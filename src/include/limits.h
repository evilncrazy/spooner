#ifndef SPOONER_LIMITS_H
#define SPOONER_LIMITS_H

/* maximum length of token literals */
#define MAX_NUMERIC_TOKEN_SIZE 128
#define MAX_NAME_TOKEN_SIZE 128

/* maximum number of objects in the evaluation stack */
#define MAX_OBJECT_STACK_SIZE 128

/* max number of variables in a call frame */
#define MAX_SCOPE_BUFFER_SIZE 128

/* maximum number of args in a function */
#define MAX_FUNCTION_ARGS 8

/* maximum number of characters in one line of code */
#define MAX_LINE_BUFFER_SIZE 128

/* maximum number of opcodes in one line of code */
#define MAX_LINE_NUM_OPCODES 128

/* maximum number of characters in an error message */
#define MAX_ERROR_MSG_LENGTH 128

#endif
