#ifndef ERRORS_H
#define ERRORS_H

typedef struct {
   const char *message;
   int line_no;
   int col_no;
} GaRuntimeError;

GaRuntimeError *ga_runtime_error(const char *msg) {
   GaRuntimeError *err = (GaRuntimeError *)malloc(sizeof(GaRuntimeError));
   err->message = msg;
   return err;
}

#endif
