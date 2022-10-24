#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include "tokenizer.h"
#include <stdlib.h>
#include <stdio.h>

// Takes a list of tokens from a Scheme program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens){
  int parenNum = 0;
  Value *stack = makeNull();
  while(tokens->type == CONS_TYPE){
    Value *curr = car(tokens);
    if(curr->type == OPEN_TYPE){
      parenNum++;
      stack = cons(curr, stack);
    }
    else if(curr->type != CLOSE_TYPE){
      stack = cons(curr, stack);
    }
    else{
      //stack = cons(curr, stack);
      parenNum--;
      Value *subList = makeNull();
      while(car(stack)->type != OPEN_TYPE){
        subList = cons(car(stack), subList);
        stack = cdr(stack);
        if(stack->type != CONS_TYPE){
          printf("Syntax error");
          texit(1);
        }
      }
      //subList = cons(cdr(stack), subList);
      //subList = reverse(subList);
      stack = cons(subList, cdr(stack));
    }
    tokens = cdr(tokens);
  }
  if(parenNum > 0){
    printf("Syntax error: not enough close parentheses\n");
    texit(1);
  }
  else if(parenNum < 0){
    printf("Syntax error: too many close parentheses\n");
    texit(1);
  }
  return stack;
}

// Prints the tree to the screen in a readable fashion. It should look just like
// Scheme code; use parentheses to indicate subtrees.
void printTree(Value *tree){
  //displayTokens(tree);
  while(tree->type == CONS_TYPE){
    switch(car(tree)->type){
      case CONS_TYPE:
        printf("(");
        printTree(car(tree));
        printf(")");
        break;
      case OPEN_TYPE:
        printf("(");
        break;
      case CLOSE_TYPE:
        printf(")");
        break;
      case INT_TYPE:
        printf("%i",car(tree)->i);
        break;
      case DOUBLE_TYPE:
        printf("%f",car(tree)->d);
        break;
      case SYMBOL_TYPE:
        printf("%s", car(tree)->s);
        break;
      case NULL_TYPE:
        printf("");
        break;
      case BOOL_TYPE:
        if (car(tree)->i == 1){
          printf("#t");
        }
        else{
          printf("#f");
        }
        break;
      case PTR_TYPE:
        printf("pointer");
        break;
      case STR_TYPE:
        printf("%s", car(tree)->s);
        break;
      default:
        printf("other");
        break;
    }
    printf(" ");
    tree = cdr(tree);
  }
}
//INT_TYPE, DOUBLE_TYPE, STR_TYPE, CONS_TYPE, NULL_TYPE, PTR_TYPE,
//OPEN_TYPE, CLOSE_TYPE, BOOL_TYPE, SYMBOL_TYPE,