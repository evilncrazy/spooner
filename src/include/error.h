#ifndef SPOONER_ERROR_H
#define SPOONER_ERROR_H

#include <cstdarg>

#include <string>

/* some macros */
#define PARSE_ERROR(msg) new SpError(ERROR_PARSE, msg)
#define RUNTIME_ERROR(msg) new SpError(ERROR_RUNTIME, msg)

#define PARSE_ERROR_F(msg, ...) new SpError(ERROR_PARSE, msg, __VA_ARGS__)
#define RUNTIME_ERROR_F(msg, ...) new SpError(ERROR_RUNTIME, msg, __VA_ARGS__) 

#define NO_ERROR NULL 

/* different types of errors */
enum ErrorType { ERROR_PARSE, ERROR_RUNTIME };

class SpError {
  private:
   const int type_;
   std::string message_;
  public:
   SpError(const int type, const std::string message);
   SpError(const int type, const char *format, ...);

   int type() const { return type_; }
   std::string message() const { return message_; }
};

#endif
