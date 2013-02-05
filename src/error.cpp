#include "include/error.h"
#include "include/limits.h"

#include <string>
#include <cstdio>

SpError::SpError(const int type, const std::string message) : type_(type), message_(message) { }

SpError::SpError(const int type, const char *format, ...) : type_(type) {
   char buffer[MAX_ERROR_MSG_LENGTH];

   va_list args;
   va_start(args, format);
   vsnprintf(buffer, MAX_ERROR_MSG_LENGTH, format, args);
   va_end(args);

   message_ = std::string(buffer);
}
