// scope_stack.h
#ifndef SCOPE_STACK_H
#define SCOPE_STACK_H

#include "scope.h"

#define MAX_SCOPE_DEPTH 32

typedef struct {
    Scope stack[MAX_SCOPE_DEPTH];
    int top;
} ScopeMetaStack;

void scope_push(ScopeMetaStack* s, const Scope * scope);
void scope_pop(ScopeMetaStack* s);
const Scope * scope_peek(const ScopeMetaStack* s);

#endif // SCOPE_STACK_H