#ifndef SPOONER_ERRORS_H
#define SPOONER_ERRORS_H

/* some macros */
#define PARSE_ERROR(msg, ...) sp_parser_error(sp_strf(msg, ##__VA_ARGS__))
#define RUNTIME_ERROR(msg, ...) sp_runtime_error(sp_strf(msg, ##__VA_ARGS__))
#define NO_ERROR NULL

/* different types of errors */
enum { ERROR_PARSE, ERROR_RUNTIME };

/* represents a runtime error */
typedef struct {
   int type; /* type of error e.g. parse, runtime */
   char *message;
   int line_no;
   int col_no;
} SpError;

/* creates a runtime error */
SpError *sp_runtime_error(char *msg) {
   SpError *err = (SpError *)sp_malloc(sizeof(SpError));
   err->type = ERROR_RUNTIME;
   err->message = msg;
   return err;
}

/* creates a parser error */
SpError *sp_parser_error(char *msg) {
   SpError *err = (SpError *)sp_malloc(sizeof(SpError));
   err->type = ERROR_PARSE;
   err->message = msg;
   return err;
}

/* free a dynamically allocated error message */
void sp_free_error(SpError *err) {
   sp_free(err->message);
   sp_free(err);
}

#endif
