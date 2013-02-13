#include "include/function.h"

SpFunction::SpFunction(const SpObject *pattern) : 
   SpGCObject(T_FUNCTION), native_(false), pattern_(pattern) { }

SpFunction::SpFunction(const SpObject *pattern, const SpFunction *quotation) :
   SpGCObject(T_FUNCTION), native_(false), pattern_(pattern), bytecode_(quotation->cbegin(), quotation->cend()) { }

SpFunction::SpFunction(const SpObject* pattern, const TokenIter begin, const TokenIter end) :
   SpGCObject(T_FUNCTION), native_(false), pattern_(pattern), bytecode_(begin, end) { }

SpFunction::~SpFunction() {
   // delete the patterns and bytecode
   delete pattern_; pattern_ = NULL;
   for (size_t i = 0; i < bytecode_.size(); i++) delete bytecode_[i], bytecode_[i] = NULL;
   bytecode_.clear();
}

SpNativeFunction::SpNativeFunction(const SpObject *pattern) : SpFunction(pattern) {
   native_ = true;
}

SpNativeAdd::SpNativeAdd() : SpNativeFunction(
   make_list({ new SpName("a"), new SpName("b") })) {
}

SpError *SpNativeAdd::native_eval(SpEnv *env) const {
   env->bind_name("$$", new SpIntValue(
      ((SpIntValue *)env->resolve_name("a"))->value() + 
      ((SpIntValue *)env->resolve_name("b"))->value()));
   return NO_ERROR;
}
