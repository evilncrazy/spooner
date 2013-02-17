#ifndef SPOONER_REPL_H
#define SPOONER_REPL_H

#include "error.h"

#include <iostream>
#include <string>

class Repl {
  public:
   /* keep reading from a stream until the code is well formed i.e.
      has balanced brackets and braces */
   static std::string read_until_complete(std::istream &stream, bool interactive = false);
};

#endif
