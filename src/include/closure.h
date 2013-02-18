#ifndef SPOONER_CLOSURE_H
#define SPOONER_CLOSURE_H

#include <string>

#include "env.h"
#include "object.h"
#include "function.h"

class SpClosure : public SpFunction {
  private:
   SpEnv *closed_env_;  // the environment which this closure closes over
  public:
   SpClosure(const ArgList args = ArgList(), const SpList *pattern = NULL,
      const SpExpr *expr = NULL);

   SpEnv *env() const { return closed_env_; }

   void close_over(const std::string &name, const SpObject *obj);
};

#endif
