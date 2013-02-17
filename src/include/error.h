#ifndef SPOONER_ERROR_H
#define SPOONER_ERROR_H

#include <cstdarg>

#include <string>

/* some macros */
#define PARSE_ERROR(msg) throw SpParseError(msg)
#define RUNTIME_ERROR(msg) throw SpRuntimeError(msg)

#define PARSE_ERROR_F(msg, ...) throw SpParseError(msg, __VA_ARGS__)
#define RUNTIME_ERROR_F(msg, ...) throw SpRuntimeError(msg, __VA_ARGS__) 

class SpRuntimeError : std::exception {
  private:
   std::string message_;
  public:
   SpRuntimeError(const std::string message);
   SpRuntimeError(const char *format, ...);

   const char *what() const throw() { return message_.c_str(); }
};

class SpParseError : std::exception {
  private:
   std::string message_;
  public:
   SpParseError(const std::string message);
   SpParseError(const char *format, ...);

   const char *what() const throw() { return message_.c_str(); }
};

#endif
