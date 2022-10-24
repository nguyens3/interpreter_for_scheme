#include <stdio.h>
#include "tokenizer.h"
#include "value.h"
#include "linkedlist.h"
#include "parser.h"
#include "talloc.h"
#include <string.h>
#include "interpreter.h"



/**
* Prints "Evaluation error." and then cleans everything 
* up and quits.
*/
void evaluationError(){
  printf("Evaluation error.");
  texit(1);
}



/**
* Finds the value mapped to a variable inside a frame 
* or the parents of that frame.
* @param tree the symbol variable
* @param frame the lowest level frame that you want to * search
* @return the value that corresponds to the input 
* symbol
*/
Value *lookUpSymbol(Value *tree, Frame *frame){
  Value *output = NULL;
  //display(tree);

  while(frame != NULL && output == NULL){
    output = lookUpSymbolHelper(tree, frame);
    frame = frame->parent;
  }
  if(output == NULL){
    //printf("\nError in lookUpSymbol: NULL output.\n");
    evaluationError();
  }
  return output;
}



/**
* Searches a single frame to see if it has a value 
* bound to the input variable
* @param tree the input variable
* @param frame the frame you want to search
* @return the value mapped to that variable
*/
Value *lookUpSymbolHelper(Value *tree, Frame *frame){
  Value *checking = frame->bindings;
  while(checking->type == CONS_TYPE && car(checking)->type != NULL_TYPE){
    if(car(checking)->type == CONS_TYPE && !strcmp(car(car(checking))->s, tree->s)){
      return cdr(car(checking));
    }
    checking = cdr(checking);
  }
  return NULL;
}



/**
* Evaluates an if statement. Evaluates the first 
* argument, then returns the second argument if true or * the third argument if false.
* @param args the CONS list of arguments.
* @param frame the frame in which the first argument 
* will be evaluated.
* @returns the second argument if the first is true,
* and the third argument if the first is false.
*/
Value *evalIf(Value *args, Frame *frame){
  if(length(args) != 3){
    printf("Error in evalIf: Length.\n");
    evaluationError();
  }
  Value *checking =  eval(car(args), frame);
  if(checking->type == BOOL_TYPE && checking->i == 1){
    return eval(car(cdr(args)), frame);
  }
  else if (checking->type == BOOL_TYPE){
    return eval(car(cdr(cdr(args))), frame);
  }
  printf("Error in evalIf\n");
  evaluationError();
  return args;
}



/**
* Evaluates a let statement. Takes in one or several 
* lists of arguments where the first argument of each 
* sublist is the variable name and the second is the
* value to be mapped onto that name. Stores these
* mappings in the bindings of the input frame.
* @param args the list or lists of variables and 
* mapping values.
* @param frame the frame in which the bindings will be
* stored.
* @return the evaluation of whatever happens after the
* bindings. 
*/
Value *evalLet(Value *args, Frame *frame){
  if(args->type != CONS_TYPE){
    //printf("\nError in evalLet: args->type.\n");
    evaluationError();
  }
  Value *next = cdr(args);
  if(next->type == NULL_TYPE){
    //printf("\nError in evalLet: next->type.\n");
    evaluationError();
  }
  args = car(args);
  if(args->type != CONS_TYPE && args->type != NULL_TYPE){
    //printf("\nError in evalLet: args->type.\n");
    evaluationError();
  }
  while(args->type != NULL_TYPE){
    Value *subArgs = car(args);
    Value *newVal;
    if(subArgs->type != CONS_TYPE){
      //printf("\nError in evalLet: subArgs->type.\n");
      evaluationError();
    }
    if(length(subArgs) != 2){
      //printf("\nError in evalLet: length(subArgs).\n");
      evaluationError();
    }
    if(car(subArgs)->type != SYMBOL_TYPE){
      //printf("\nError in evalLet: car(subArgs)->type.\n");
      evaluationError();
    }
    if(lookUpSymbolHelper(car(subArgs), frame) != NULL){
      //printf("\nError in evalLet: No match.\n");
      evaluationError();
    }
    newVal = cons(car(subArgs), eval(car(cdr(subArgs)), frame->parent));
    frame->bindings = cons(newVal, frame->bindings);
    args = cdr(args);
  }
  Value *output;
  //printf("\nlet1\n");
  while(next->type != NULL_TYPE){
    // printf("\nnext:\n");
    // display(next);
    output = eval(car(next), frame);
    // printf("\noutput:\n");
    // display(output);
    next = cdr(next);
  }
  //printf("\nlet2\n");
  return output;
}



/**
* Returns the input without evaluating it.
* @param args whatever you want to return without 
* evaluating.
* @return the unevaluated args.
*/
Value *evalQuote(Value *args){
  if(length(args) == 0){
    //printf("\nError in evalQuote: Length.\n");
    evaluationError();
  }
  args = car(args);
  return args;
}



/**
* Evaluates a define statement by mapping values onto 
* variables and storing those bindings in the highest 
* level frame.
* @param args the variables and values you want to map.
* @param frame the highest parent for this input frame
* will be where you set the mappings.
* @return an empty void value.
*/
Value *evalDefine(Value *args, Frame *frame){
  if(length(args) < 2){
    //printf("\nError in evalDefine: Length.\n");
    evaluationError();
  }
  while(frame->parent != NULL){
    frame = frame->parent;
  }
  if(car(args)->type != SYMBOL_TYPE){
    //printf("\nError in evalDefine: Type.\n");
    evaluationError();
  }
  Value *newVal = cons(car(args), eval(car(cdr(args)), frame)); 
  frame->bindings = cons(newVal, frame->bindings);
  return makeVoid();
}



/**
* Checks to see if the parameter names are valid. If 
* not, triggers an evaluation error.
* @param args a list of parameter names you want to 
* check.
*/
void checkParamNames(Value *args){
  while(args->type == CONS_TYPE){
    Value *testing = car(args);
    if(testing->type == CONS_TYPE){
      checkParamNames(testing);
    }
    else if(testing->type == SYMBOL_TYPE){
      Value *others = cdr(args);
      while(others->type == CONS_TYPE){
        while(car(others)->type == CONS_TYPE){
          others = car(others);
        }
        if(car(others)->type != SYMBOL_TYPE){
          //printf("\nError in checkParamNames: Car type.\n");
          evaluationError();
        }
        if(!strcmp(testing->s, car(others)->s)){
          //printf("\nError in checkParamNames: No match.\n");
          evaluationError();
        }
        others = cdr(others);
      }
    }
    else{
      //printf("\nError in checkParamNames: Type.\n");
      evaluationError();
    }
    args = cdr(args);
  }
}



/**
* eval lambda
*/
Value *evalLambda(Value *args, Frame *frame){
  if(length(args) < 2){
    //printf("\nError in evalLambda: Length.\n");
    evaluationError();
  }
  Value *output = talloc(sizeof(Value));
  output->type = CLOSURE_TYPE;
  Value *paramNames = car(args);
  checkParamNames(paramNames);
  output->cl.paramNames = car(args);
  args = cdr(args);
  output->cl.functionCode = car(args); 
  output->cl.frame = frame;
  return output;
}

Value *apply(Value *function, Value *args){
  // printf("\n\n\n\n");
  // printf("apply:\n");
  // display(function);
  // printf("\n");
  // display(args);
  // printf("\n\n\n\n");

  if(function->type == CLOSURE_TYPE){

    Frame *newFrame = talloc(sizeof(Frame));
    newFrame->bindings = makeNull();
    newFrame->parent = function->cl.frame;

    Value *checking = function->cl.paramNames;

    while(args->type == CONS_TYPE && checking->type == CONS_TYPE){
      Value *temp = cons(car(checking), car(args));
      newFrame->bindings = cons(temp, newFrame->bindings);
      checking = cdr(checking);
      args = cdr(args);
    }

    return eval(function->cl.functionCode, newFrame);
  }
  else if(function->type == PRIMITIVE_TYPE){
    return function->pf(args);
  }

  return function;
} 



/**
*
*/
Value *evalLetStar(Value *args, Frame *frame){
  if(args->type != CONS_TYPE){
    //printf("\nError in evalLetStar: args->type.\n");
    evaluationError();
  }
  Value *next = cdr(args);
  if(next->type == NULL_TYPE){
    //printf("\nError in evalLetStar: next->type.\n");
    evaluationError();
  }
  args = car(args);
  if(args->type != CONS_TYPE && args->type != NULL_TYPE){
    //printf("\nError in evalLetStar: args->type.\n");
    evaluationError();
  }
  while(args->type != NULL_TYPE){
    Value *subArgs = car(args);
    Value *newVal;
    if(subArgs->type != CONS_TYPE){
      //printf("\nError in evalLetStar: subArgs->type.\n");
      evaluationError();
    }
    if(length(subArgs) != 2){
      //printf("\nError in evalLetStar: length(subArgs).\n");
      evaluationError();
    }
    if(car(subArgs)->type != SYMBOL_TYPE){
      //printf("\nError in evalLetStar: car(subArgs)->type.\n");
      evaluationError();
    }
    if(lookUpSymbolHelper(car(subArgs), frame) != NULL && lookUpSymbol(car(subArgs), frame) == NULL){
      //printf("\nError in evalLetStar: No match.\n");
      evaluationError();
    }
    Frame *newFrame = talloc(sizeof(Frame));
    newVal = cons(car(subArgs), eval(car(cdr(subArgs)), frame));
    newFrame->bindings = cons(newVal, makeNull());
    newFrame->parent = frame;
    frame = newFrame;
    args = cdr(args);
  }
  Value *output;
  while(next->type != NULL_TYPE){
    output = eval(car(next), frame);
    next = cdr(next);
  }
  return output;
}



/**
*
*/
Value *evalLetrec(Value *args, Frame *frame){
  
  if(args->type != CONS_TYPE){
    //printf("\nError in evalLetrec: args->type.\n");
    evaluationError();
  }
  Value *next = cdr(args);
  if(next->type == NULL_TYPE){
    //printf("\nError in evalLetrec: next->type.\n");
    evaluationError();
  }
  args = car(args);
  if(args->type != CONS_TYPE && args->type != NULL_TYPE){
    //printf("\nError in evalLetrec: args->type.\n");
    evaluationError();
  }


  Frame *tempFrame = talloc(sizeof(Frame));
  tempFrame->parent = frame;
  tempFrame->bindings = makeNull();



  while(args->type != NULL_TYPE){
    Value *subArgs = car(args);
    Value *newVal = cons(car(subArgs), car(cdr(subArgs)));
    tempFrame->bindings = cons(newVal, tempFrame->bindings);
    args = cdr(args);
  }


  Frame *newFrame = talloc(sizeof(Frame));
  newFrame->parent = frame;
  newFrame->bindings = makeNull();
  args = tempFrame->bindings;



  while(args->type != NULL_TYPE){
    Value *subArgs = car(args);
    Value *secondVal = eval(cdr(subArgs), newFrame); //changed to newframe
    Value *newVal = cons(car(subArgs), secondVal);
    newFrame->bindings = cons(newVal, newFrame->bindings);
    args = cdr(args);
  }

  Value *output = eval(next, newFrame);
  return output;
}



/**
*
*/
Value *evalSet(Value *args, Frame *frame){
  if (length(args) == 2){
    Frame *global = frame;
    while (global->parent != NULL){
      global = global->parent;
    }
    Value *curr = car(args);
    Value *body = car(cdr(args));
    Value *newBind = cons(curr, eval(body, frame)); //changed this
    Value *newBindings = makeNull();
    if (curr->type == SYMBOL_TYPE){
      Value *curBind = global->bindings;
      while (curBind->type != NULL_TYPE){
        Value *symbol = car(car(curBind));
        frame->bindings = cons(newBind, frame->bindings);
        newBindings = cons(newBind, newBindings);
        curBind = cdr(curBind);
      }
      global->bindings = cons(newBindings, global->bindings);
      Value *returnVal = talloc(sizeof(Value));
      returnVal->type = VOID_TYPE;
      return returnVal;
    }
  }
  printf("Set Evaluation Error");
  evaluationError();
  return NULL; 
}



/**
*
*/
Value *evalBegin(Value *args, Frame *frame){
  Value *body = args;
  Value *curValue = talloc(sizeof(Value));
  if (body->type == NULL_TYPE){
	  return makeVoid();
  }
  while (body->type != NULL_TYPE) {
	  curValue = eval(car(body),frame);
	  body = cdr(body); 
  } 
  return curValue;  
}



/**
*
*/
Value *evalAnd(Value *args, Frame *frame){
     Value *result = talloc(sizeof(Value));
    result->type = BOOL_TYPE;
    Value *body = args;
    while (cdr(body)->type != NULL_TYPE) {
        Value *curValue = eval(car(body), frame);
        if (curValue->type == BOOL_TYPE && curValue->i == 0) {
            result->s = "#f";
            return result;
        }
        body = cdr(body);
    }
    return eval(car(body),frame);
}



/**
*
*/
Value *evalOr(Value *args, Frame *frame){
  Value *result = talloc(sizeof(Value));
  result->type = BOOL_TYPE;
  Value *body = args;
  while (cdr(body)->type != NULL_TYPE) {
    Value *curValue = eval(car(body), frame);
    if (curValue->type == BOOL_TYPE && curValue->i == 1){
      return curValue;
    }
    body = cdr(body);
  }
  return eval(car(body),frame);
}


/*
* Adds a binding between the given name
* and the input function. Used to add
* bindings for primitive funtions to the top-level
* bindings list.
* @param name the string you want to name this function.
* @param (*function)(Value *) a pointer to the function
* code itself, which will take in a Value * argument.
* @param frame the highest level frame in which this 
* function will be bound to its name.
*/
void bind(char *name, Value *(*function)(Value *), Frame *frame) {
  Value *myFunction = talloc(sizeof(Value));
  myFunction->type = PRIMITIVE_TYPE;
  myFunction->pf = function;
  Value *myName = talloc(sizeof(Value));
  myName->type = SYMBOL_TYPE;
  myName->s = name;
  Value *binding = cons(myName, myFunction);
  frame->bindings = cons(binding, frame->bindings);
}


/**
* Evaluates a cons statement by checking the length and
* then placing the first and second arguments into a
* cons cell.
* @param args the arguments you want to cons together
* @return the output cons cell.
*/
Value *evalCons(Value *args){
  if(length(args) != 2){
    //printf("\nError in evalCons: Length.\n");
    evaluationError();
  }
  Value *first = car(args);
  Value *second = car(cdr(args));
  return cons(first, second);
}



/**
* Evaluates cdr by checking to see that args is of an 
* appropriate length and then returning the cdr of 
* args.
* @param args the cons value that you want to cdr of.
* @return the cdr of args.
*/
Value *evalCdr(Value *args){
  if(length(args) != 1){
    //printf("\nError in evalCdr: Length.\n");
    evaluationError();
  }
  if(args->type != CONS_TYPE || car(args)->type != CONS_TYPE){
    //printf("\nError in evalCdr: Type.\n");
    evaluationError();
  }

  return cdr(car(args)); 
}



/**
* Evaluates car by checking to see that args is of an 
* appropriate length and then returning the car of 
* args.
* @param args the cons value that you want to car of.
* @return the car of args.
*/
Value *evalCar(Value *args){
  if(length(args) != 1){
    //printf("\nError in evalCar: Length.\n");
    evaluationError();
  }
  if(args->type != CONS_TYPE || car(args)->type != CONS_TYPE){
    //printf("\nError in evalCar: Type.\n");
    evaluationError();
  }
  return car(car(args)); 
}


/**
* Checks to see if args is a null type. 
* @param args the value that you're checking.
* @return true if args is a null type, and false if it
* isn't.
*/
Value *evalNull(Value *args){
  if(length(args) != 1){
    //printf("\nError in evalNull: Length.\n");
    evaluationError();
  }
  Value *output = talloc(sizeof(Value));
  output->type = BOOL_TYPE;
  while(args->type == CONS_TYPE){
    args = car(args);
  }
  if(args == NULL || args->type == NULL_TYPE){
    output->i = 1;
  }
  else{
    output->i = 0;
  }
  return output;
}


/**
* Evaluates a plus statement by adding all the following
* values and returning the value containing the sum.
* If any of the values are doubles, sum contains a 
* float. Otherwise, sum is an int.
* @param args the values you want to add up.
* @return a value containing the sum of all args.
*/
Value *evalPlus(Value *args){
  // display(args);
  // printf("\n");
  Value *output = talloc(sizeof(Value));
  bool isInt = true;
  int intSum = 0;
  float floatSum;
  while(args->type == CONS_TYPE){
    Value *subArg = car(args);
    while(subArg->type == CONS_TYPE){
      subArg = car(subArg);
    }
    if(isInt && subArg->type == DOUBLE_TYPE){
      isInt = false;
      floatSum = (float)intSum;
    }
    else if(subArg->type != INT_TYPE && subArg->type != DOUBLE_TYPE){
      //printf("\nError in evalPlus: Type.\n");
      evaluationError();
    }
    if (isInt){
      intSum += subArg->i;
      //printf("%i\n", intSum);
    }
    else{
      floatSum += subArg->d;
      //printf("%f\n", floatSum);
    }
    args = cdr(args);
  }
  if(isInt){
    output->type = INT_TYPE;
    output->i = intSum;
  }
  else{
    output->type = DOUBLE_TYPE;
    output->d = floatSum;
  }
  return output;
}



/**
* evalMinus
*/
Value *evalMinus(Value *args){
  Value *output = talloc(sizeof(Value));
  bool isInt = true;
  int intSum;
  float floatSum;
  bool first = true;
  

  while(args->type == CONS_TYPE){
    Value *subArg = car(args);
    while(subArg->type == CONS_TYPE){
      subArg = car(subArg);
    }
    if(first){
      if(subArg->type == INT_TYPE){
      intSum = subArg->i;
      }
      else if(subArg->type == DOUBLE_TYPE){
        isInt = false;
        floatSum = subArg->d;
      }
      first = false;
    }
    else{
      if(isInt && subArg->type == DOUBLE_TYPE){
        isInt = false;
        floatSum = (float)intSum;
      }
      else if(subArg->type != INT_TYPE && subArg->type != DOUBLE_TYPE){
        //printf("\nError in evalPlus: Type.\n");
        evaluationError();
      }
      if (isInt){
        intSum -= subArg->i;
        //printf("%i\n", intSum);
      }
      else{
        floatSum -= subArg->d;
        //printf("%f\n", floatSum);
      }
    }
    args = cdr(args);
  }
  if(isInt){
    output->type = INT_TYPE;
    output->i = intSum;
  }
  else{
    output->type = DOUBLE_TYPE;
    output->d = floatSum;
  }
  return output;
}



/**
* evalEquals :) fun fact this only works for ints
*/
Value *evalEquals(Value *args){
  Value *output = talloc(sizeof(Value));
  output->type = BOOL_TYPE;
  if(car(args)->i == car(cdr(args))->i){
    output->i = 1;
  }
  else{
    output->i = 0;
  }
  return output;
}



/**
* evalLessThan
*/
Value *evalLessThan(Value *args){
  Value *output = talloc(sizeof(Value));
  output->type = BOOL_TYPE;
  if(length(args) != 2){
    printf("Error in evalLessThan: Length.");
    evaluationError();
  }
  Value *first = car(args);
  Value *second = car(cdr(args));
  double firstVal;
  double secondVal;
  if(first->type == INT_TYPE){
    firstVal = (double) first->i;
  }
  else{
    firstVal = first->d;
  }
  if(second->type == INT_TYPE){
    secondVal = (double) second->i;
  }
  else{
    secondVal = second->d;
  }

  if(firstVal < secondVal){
    output->i = 1;
  }
  else{
    output->i = 0;
  }

  
  return output;
}

Value *evalGreaterThan(Value *args){
  Value *output = talloc(sizeof(Value));
  output->type = BOOL_TYPE;
  if(length(args) != 2){
    printf("Error in evalGreaterThan: Length");
    evaluationError();
  }
  Value *first = car(args);
  Value *second = car(cdr (args));
  double firstVal;
  double secondVal;
  if(first->type == INT_TYPE){
    firstVal = (double)first->i;
  } 
  else{
    firstVal = first->d;
  }
  if(second->type == INT_TYPE){
    secondVal = (double)second->i;
  } 
  else{
    secondVal = second->d;
  }
  
  if(firstVal > secondVal){
    output->i = 1;
  }
  else{
    output->i = 0;
  }

  return output;
}


/**
* Evaluates an expression within a specific frame.
* @param expr the s expression you want to evaluate.
* @param frame the frame in which you are evaluating it.
* @return the output you get after fully evaluating 
* that s expression.
*/
Value *eval(Value *expr, Frame *frame){
  //printf("\n\n");
  //display(expr);
  //printf("\n");
  Value *tree = expr;
  // printf("\n\n");
  // display(tree);
  // printf("\n\n");
  switch (tree->type)  {
    case INT_TYPE: {
      return tree;
      break;
    }
    case DOUBLE_TYPE:{
      return tree;
      break;
    }
    case BOOL_TYPE: {
      return tree;
      break;
    }  
    case STR_TYPE:{
      return tree;
      break;
    }
    case SYMBOL_TYPE: {
      return lookUpSymbol(tree, frame);
      break;
    }
    case NULL_TYPE:{
      return tree;
      break;
    }  
    case CLOSURE_TYPE:{
      return tree;
      break;
    }
    case PRIMITIVE_TYPE:{
      return tree;
      break;
    }
    case VOID_TYPE:{
      return tree;
      break; //added this
    }
    case CONS_TYPE: {
      Value *first = car(tree);
      // Sanity and error checking on first...
      if(first->type == STR_TYPE){
        return eval(first, frame);
      }
      else if(first->type == SYMBOL_TYPE){
        if (!strcmp(first->s,"if")) { 
          Value *args = cdr(tree);
          Value *resultVal = evalIf(args,frame);
          return resultVal;
        }
        //keep evaluating inside of the let body is one or more s expression
        else if (!strcmp(first->s,"let")) {
          Value *args = cdr(tree);
          Frame *newFrame = talloc(sizeof(Frame));
          newFrame->parent = frame;
          Value *nullVal = makeNull();
          newFrame->bindings = nullVal;
          Value *resultVal = evalLet(args, newFrame);
          return resultVal;
        }
        else if(!strcmp(first->s,"quote")){
          Value *args = cdr(tree);
          Value *resultVal = evalQuote(args);
          return resultVal;
        }
        else if(!strcmp(first->s,"define")){
          Value *args = cdr(tree);
          Value *resultVal = evalDefine(args,frame);
          return resultVal;
        }
        else if(!strcmp(first->s,"lambda")){
          if(length(tree) < 2){
            printf("evaluation error in lambda in actual eval");
            evaluationError();
          }
          Value *args = cdr(tree);
          Value *resultVal = evalLambda(args,frame);
          return resultVal;
        }
        else if(!strcmp(first->s, "let*")){
          Value *args = cdr(tree);
          Value *resultVal = evalLetStar(args,frame);
          return resultVal;
        }
        else if(!strcmp(first->s, "letrec")){
          Value *args = cdr(tree);
          Value *resultVal = evalLetrec(args,frame);
          return resultVal;
        }
        else if(!strcmp(first->s,"set!")){
          Value *args = cdr(tree);
          Value *resultVal = evalSet(args,frame);
          //printf("\nset done\n");
          return resultVal;
        }
        else if(!strcmp(first->s,"and")){
          Value *args = cdr(tree);
          Value *resultVal = evalAnd(args,frame);
          return resultVal;
        }
        else if(!strcmp(first->s,"or")){
          Value *args = cdr(tree);
          Value *resultVal = evalOr(args,frame);
          return resultVal;
        }
        else if(!strcmp(first->s,"begin")){
          Value *args = cdr(tree);
          Value *resultVal = evalBegin(args,frame);
          return eval(resultVal, frame); //THIS LINE
        }
        else{
          Value *output = eval(first, frame);
          if(output->type != CLOSURE_TYPE && output->type != PRIMITIVE_TYPE){
            return output;
          }
          else{
            Value *args = cdr(tree);
            Value *input = makeNull();
            while(args->type == CONS_TYPE){
              input = cons(eval(car(args), frame), input);
              args = cdr(args);          
            }
            input = reverse(input);
            return apply(eval(first, frame), input);
          }
        }
      }
      else{
        Value *args = cdr(tree);
        Value *input = makeNull();
        while(args->type == CONS_TYPE){
          input = cons(eval(car(args), frame), input);
          args = cdr(args);          
        }
        input = reverse(input);
        return apply(eval(first, frame), input);
      }

      // .. other special forms here...
      break;
    }
    default:
      evaluationError();
      texit(1);
      break;
  }    
  return cdr(expr);
}



/**
* Interprets scheme code by binding primitive functions
* to a parent frame and then evaluating a series of
* s expressions and displaying the results for each
* s expression.
* @param tree the full tree of s expressions you want
* to interpret.
*/
void interpret(Value *tree){
  Frame *frame = talloc(sizeof(Frame));
  tree = reverse(tree);
  frame->parent = NULL;
  Value *nullVal = makeNull();
  frame->bindings = nullVal;
  //bind stuff here
  bind("+", &evalPlus, frame);
  bind("-", &evalMinus, frame);
  bind("=", &evalEquals, frame);
  bind("<", &evalLessThan, frame);
  bind(">", &evalGreaterThan, frame);
  bind("null?", &evalNull, frame);
  bind("car", &evalCar, frame);
  bind("cdr", &evalCdr, frame);
  bind("cons", &evalCons, frame);

  while(tree->type == CONS_TYPE){
    display(eval(car(tree), frame));
    tree = cdr(tree);
  }
}



/**
* Displays any s expression through print statements.
* @param val an s expression you want to display.
*/
void display(Value *val){

  switch (val->type) {
      case INT_TYPE:
      printf("%i ", val->i);
      break;
    case PTR_TYPE:
      printf("%p ", val->p);
      break;
    case DOUBLE_TYPE:
      printf("%f ",val->d);
      break;
    case STR_TYPE:
      printf("%s ", val->s);
      break;
    case NULL_TYPE:
      printf("()");
      break;
    case VOID_TYPE:
      break;
    case CLOSURE_TYPE:
      printf("#<procedure>\n");
      break;
    case PRIMITIVE_TYPE:
      printf("#primitive\n");
      break;
    case CONS_TYPE:
        printf("(");
        while(val->type == CONS_TYPE){
        display(val->c.car);
        val = cdr(val);
        }
        if(val->type != NULL_TYPE){
          printf(". ");
          display(val);
        }
        printf(") ");
      
      break;
    case BOOL_TYPE:
      if(val->i == 1){
        printf("#t ");
      }
      else{
        printf("#f ");
      }
      break;
    case SYMBOL_TYPE:
      printf("%s ", val->s);
      break;
    default:
      printf("other");
      break;
    }
}



/**
* Displays a single value through print statements.
* @param val the value you want to display.
*/
void display1(Value *val){
  switch (val->type) {
      case INT_TYPE:
      printf("%i ", val->i);
      break;
    case PTR_TYPE:
      printf("%p ", val->p);
      break;
    case DOUBLE_TYPE:
      printf("%f ",val->d);
      break;
    case STR_TYPE:
      printf("%s ", val->s);
      break;
    case CONS_TYPE:
      printf("cons");
      break;
    case BOOL_TYPE:
      if(val->i == 1){
        printf("#t ");
      }
      else{
        printf("#f ");
      }
      break;
    case SYMBOL_TYPE:
      printf("%s ", val->s);
      break;
    case OPEN_TYPE:
      printf("open");
      break;
    case CLOSE_TYPE:
      printf("close");
      break;
    case NULL_TYPE:
      printf("null");
      break;
    case CLOSURE_TYPE:
      printf("closure?");
      break;
    default:
      printf("other");
      break;
    }
}

//common time 1 to 1:30am