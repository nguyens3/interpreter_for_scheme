#ifndef _INTERPRETER
#define _INTERPRETER

void interpret(Value *tree);
Value *eval(Value *expr, Frame *frame);
Value *lookUpSymbolHelper(Value *tree, Frame *frame);

void display1(Value *val);

#endif

