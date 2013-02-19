#ifndef SPOONER_REPL_H
#define SPOONER_REPL_H

#include "error.h"

#include <cstdio>
#include <string>

class Repl {
  public:
   /* keep reading from a file until the code is well formed i.e.
      has balanced brackets and braces */
   static std::string read_until_complete(FILE *f, bool interactive = false);
};

#endif
