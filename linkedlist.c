#include <stdbool.h>
#include "value.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "talloc.h"


// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list){
 assert(list->type == CONS_TYPE && "Error (type): input list is not CONS_TYPE");
  assert(list != NULL && "Error (length): input list is NULL");
  assert(list->c.car != NULL && "Error: list->c.car is NULL");
  return list->c.car;
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list){
  assert(list->type == CONS_TYPE && "Error (type): input list is not CONS_TYPE");
  assert(list != NULL && "Error (length): input list is NULL");
  assert(list->c.car != NULL && "Error: list->c.car is NULL");
  assert(list->c.cdr != NULL && "Error: list->c.cdr is NULL");
  return list->c.cdr;
}

// Create a new NULL_TYPE value node.
Value *makeNull(){
  Value *nulltyp = talloc(sizeof(Value));
  nulltyp->type = NULL_TYPE;
  return nulltyp;
}

Value *makeVoid(){
  Value *output = talloc(sizeof(Value));
  output->type = VOID_TYPE;
  return output;
}

// Create a new CONS_TYPE value node.
Value *cons(Value *newCar, Value *newCdr){
  Value *newValue = talloc(sizeof(Value));
  newValue->type = CONS_TYPE;
  newValue->c.car = newCar;
  newValue->c.cdr = newCdr;
  return newValue;
}


// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value){
  
  assert(value != NULL && "Error: input value is NULL");
  if (value->type == NULL_TYPE){
    return true;
  } else{
    return false;
  }
}


// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.
int length(Value *value){
 int count = 0;
  while(!isNull(value)){
    value = cdr(value);
    count++;
  }
  return count;
}

// Display the contents of the linked list to the screen in some kind of
// readable format
// void linkedlistdisplay(Value *val){
//     switch (val->type) {
//       case INT_TYPE:
//       printf("%i\n", val->i);
//       break;
//     case PTR_TYPE:
//       printf("%p\n", val->p);
//       break;
//     case DOUBLE_TYPE:
//       printf("%f\n",val->d);
//       break;
//     case STR_TYPE:
//       printf("%s\n", val->s);
//       break;
//     case CONS_TYPE:
//       display(val->c.car);
//       display(val->c.cdr);
//       break;
//     case OPEN_TYPE:
//       printf("open\n");
//       break;
//     case CLOSE_TYPE:
//       printf("close\n");
//       break;
//     case BOOL_TYPE:
//       printf("bool\n");
//       break;
//     case SYMBOL_TYPE:
//       printf("Symbol\n");
//       break;
//     case OPENBRACKET_TYPE:
//       printf("open bracket\n");
//       break;
//     case CLOSEBRACKET_TYPE:
//       printf("close bracket\n");
//       break;
//     case NULL_TYPE:
//       printf("End!\n");
//       break;
//     default:
//       printf("other\n");
//       break;
//     }
// }

// Return a new list that is the reverse of the one that is passed in. All
// content within the list should be duplicated; there should be no shared
// memory whatsoever between the original list and the new one.
//
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
Value *reverse(Value *list){
  Value *newList;
  newList = talloc(sizeof(Value));
  newList->type = NULL_TYPE;

  while (!isNull(list)){
    // Value *newVal = talloc(sizeof(Value));
    // newVal->type = car(list)->type;
    // switch (newVal->type) {
    //   case INT_TYPE:
    //     newVal->i = car(list)->i;
    //     break;
    //   case PTR_TYPE:
    //     newVal->p = car(list)->p;
    //     break;
    //   case DOUBLE_TYPE:
    //     newVal->d = car(list)->d;
    //     break;
    //   case STR_TYPE:
    //     newVal->s = car(list)->s;
    //     break;
    //   case CONS_TYPE:
    //     newVal->c.car = car(list)->c.car;
    //     newVal->c.cdr = car(list)->c.cdr;
    //     break;
    //   case NULL_TYPE:
    //     break;
    //   case OPEN_TYPE:
    //     newVal->s = car(list)->s;
    //     break;
    //   case CLOSE_TYPE:
    //     newVal->s = car(list)->s;
    //     break;
    //   case BOOL_TYPE:
    //     newVal->i = car(list)->i;
    //     break;
    //   case SYMBOL_TYPE:
    //     newVal->s = car(list)->s;
    //     break;
    //   default:
    //     break;
    // }
    newList = cons(car(list), newList);
    // if(car(list)->type == CONS_TYPE){
    //   newList->c.car = reverse(car(newList));
    // } // added this
    list = cdr(list);
  }
  return newList;
}