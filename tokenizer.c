#include "value.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "talloc.h"
#include "linkedlist.h"
#include <ctype.h>



bool isAlpha(char constChar){
  if(constChar >='a' && constChar <='z'){
    return true;
  }
  else if(constChar >='A' && constChar <='Z'){
    return true;
  }
  else return false;
}

bool isDigit(char constChar){
  if(constChar != '\0'){
    if(constChar >='0' && constChar <='9'){
      return true;
    }
  }
  return false;
}

bool isSymbol(char constChar){
  if(constChar == '-'){
    constChar = (char)fgetc(stdin);
    if(constChar == ' ' || constChar == ')'){
      ungetc(constChar, stdin);
      return true;
    }
    else{
      ungetc(constChar, stdin);
      return false;
    }
  }
  else if(isAlpha(constChar) || constChar == '!' || (constChar >= '<' && constChar <= '>') || (constChar >= '$' && constChar <= '/')){
    return true;
  }
  return false;
}

Value *checkDouble(char constChar, Value *newVal){
  bool isDouble = false;
  int count = 0;
  char *output = talloc(sizeof(char) * 300);
  while(constChar != ' ' && constChar != '\n' && constChar != ')' && constChar != EOF){
    if(constChar == '.'){ isDouble = true;}
    output[count] = constChar;
    constChar = (char)fgetc(stdin);
    count++;
  }
  output[count] = '\0';
  if(isDouble){
    newVal->type = DOUBLE_TYPE;
    newVal->d = strtod(output, NULL);
  }
  else{
    newVal->type = INT_TYPE;
    newVal->i = strtol(output, NULL, 10);
  }
  ungetc(constChar, stdin);
  return newVal;
}

char *getFullSymbolString(char *charRead){
  char *outChar = talloc(sizeof(char) * 300);
  outChar[0] = *charRead;
  char constChar = (char)fgetc(stdin);
  int counter = 1;
  while(constChar != ' ' && constChar != ')' && constChar !='\n'){
    outChar[counter] = constChar;
    constChar = (char)fgetc(stdin);
    counter++;
  }
  ungetc(constChar, stdin);
  outChar[counter] = '\0';
  return outChar;
}

char *getFullQuoteString(char *charRead){
  char *outChar = talloc(sizeof(char) * 300);
  outChar[0] = *charRead;
  char constChar = (char)fgetc(stdin);
  int counter = 1;
  while(constChar != '\"'){
    outChar[counter] = constChar;
    constChar = (char)fgetc(stdin);
    counter++;
  }
  outChar[counter] = constChar;
  counter++;
  outChar[counter] = '\0';
  return outChar;
}


// Read all of the input from stdin, and return a linked list consisting of the
// tokens.
Value *tokenize(){
  //read in from stdin 
  //split into tokens
  //turn tokens into a linked list of values
  //output format is token:type
  // CONS_TYPE, NULL_TYPE, PTR_TYPE
  //OPEN_TYPE, CLOSE_TYPE, SYMBOL_TYPE, BOOL_TYPE, STR_TYPE, INT_TYPE, DOUBLE_TYPE
  //strcat and add on to a string
  //use an array
  //display???

  Value *list = makeNull();
  char constChar = (char)fgetc(stdin);
  char *charRead = &constChar;
  char *s = talloc(sizeof(char)*300);
  s = &constChar;
  

  
  while (constChar != EOF) {

    bool isComment = false;
    bool isSpace = false;
    Value *newVal = talloc(sizeof(Value));
   
    if (constChar =='(' ) { //Open
      newVal->type = OPEN_TYPE;
      newVal->s = talloc(sizeof(char)*2);
      newVal->s[0] = constChar;
      newVal->s[1] = '\0';
    } else if (constChar == ')') { //close
      newVal->type = CLOSE_TYPE;
      newVal->s = talloc(sizeof(char)*2);
      newVal->s[0] = constChar;
      newVal->s[1] = '\0';

    } else if (isSymbol(constChar)){ //Symbol, I dont think I got all of them
      newVal->type = SYMBOL_TYPE;
      newVal->s = getFullSymbolString(charRead);
    } else if (constChar == '#') { //Boolean
      constChar = (char)fgetc(stdin);
      if(constChar == 't'){
        newVal->type = BOOL_TYPE;
        newVal->i = 1;
      }
      else if(constChar == 'f'){
        newVal->type = BOOL_TYPE;
        newVal->i = 0;
      }
      else{
        printf("Syntax Error: Unrecognized token:%c\n", constChar);
        isComment = true;
        texit(1); //1 faliure 0 is success 
      }

    } else if (constChar == '\"') { //String
      newVal->type = STR_TYPE;
      newVal->s = getFullQuoteString(charRead);
      //fix this

    } else if (isDigit(constChar) || constChar == '-'){ // Integer or double
      //printf("in isDigit %c\n", constChar);
      newVal = checkDouble(constChar, newVal); 

    } else if (constChar == ';') { //comments
      isComment = true;
    } else if (constChar == ' ' || constChar == '\n'){
      isSpace = true;
    }
    else { //not recognized
      printf("Syntax Error: Unrecognized token:%c\n", constChar);
      isComment = true;
      texit(1); //1 faliure 0 is success 
    }

    //big if ends

    if(isComment){
      while(constChar != '\n'){
        constChar = (char)fgetc(stdin);
      }
    }
    else if(isSpace){
      constChar = (char)fgetc(stdin);
    }
    else{
      list = cons(newVal, list); // idk if this is right but i think so
      constChar = (char)fgetc(stdin);
    }

  }

  Value *revList = reverse(list);
  return revList;
}



//unget function lets you put a character back

// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list){
  while(!isNull(list)){

    switch (car(list)->type){
      case INT_TYPE:
        printf("%i", car(list)->i);
        printf(":integer\n");
        break;
      case PTR_TYPE:
        printf("%p", car(list)->p);
        printf(":pointer\n");
        break;
      case DOUBLE_TYPE:
        printf("%f",car(list)->d);
        printf(":double\n");
        break;
      case STR_TYPE:
        printf("%s", car(list)->s);
        printf(":string\n");
        break;
      case CONS_TYPE:
        displayTokens(car(list)->c.car);
        displayTokens(car(list)->c.cdr);
        break;
      case NULL_TYPE:
        printf("End!\n");
        printf(":NULL_TYPE\n");
        break;
      case OPEN_TYPE:
        printf("(:open\n");
        break;
      case CLOSE_TYPE:
        printf("):close\n");
        break;
      case SYMBOL_TYPE:
        printf("%s", car(list)->s);
        printf(":symbol\n");
        break;
      case BOOL_TYPE:
        if (car(list)->i == 1){
          printf("#t");
        }
        else{
          printf("#f");
        }
        printf(":boolean\n");
        break;
      default:
        printf("other");
        break;
    }

    list = cdr(list);
  }
  
}

