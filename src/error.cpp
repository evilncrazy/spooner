#include "include/error.h"
#include "include/limits.h"

#include <string>

SpRuntimeError::SpRuntimeError(const std::string message) : message_(message) { }

SpRuntimeError::SpRuntimeError(const char *format, ...) {
   char buffer[MAX_ERROR_MSG_LENGTH];

   va_list args;
   va_start(args, format);
   vsnprintf(buffer, MAX_ERROR_MSG_LENGTH, format, args);
   va_end(args);

   message_ = std::string(buffer);
}

SpParseError::SpParseError(const std::string message) : message_(message) { }

SpParseError::SpParseError(const char *format, ...) {
   char buffer[MAX_ERROR_MSG_LENGTH];

   va_list args;
   va_start(args, format);
   vsnprintf(buffer, MAX_ERROR_MSG_LENGTH, format, args);
   va_end(args);

   message_ = std::string(buffer);
}
