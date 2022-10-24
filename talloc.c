#include <stdlib.h>
#include "value.h"
#include <stdio.h>
#include "linkedlist.h"



//make it work with the pointer value
struct Node {
  Value *pointer;
  struct Node *next;
};

typedef struct Node Node;
Node *head = NULL;



void addNode(Node *newNode){

  // if(head == NULL){
  //   head = malloc(sizeof(Node));
  //   printf("hi\n");
  // }
  
  newNode->next = head;
  head = newNode;
}

// Replacement for malloc that stores the pointers allocated. It should store
// the pointers in some kind of list; a linked list would do fine, but insert
// here whatever code you'll need to do so; don't call functions in the
// pre-existing linkedlist.h. Otherwise you'll end up with circular
// dependencies, since you're going to modify the linked list to use talloc.
void *talloc(size_t size){
  Value *myPointer = malloc(sizeof(Value));
  myPointer->type = PTR_TYPE;
  myPointer->p = malloc(size);
  Node *myNode = malloc(sizeof(Node));
  myNode->pointer = myPointer;
  addNode(myNode);
  return myPointer->p;
}

// Free all pointers allocated by talloc, as well as whatever memory you
// allocated in lists to hold those pointers.
void tfree(){
  Node *curr = head;
  while(curr->next != NULL){
    Node *temp = curr;
    curr = curr->next;

      free(temp->pointer->p); //uninitialized value bc the last node doesn't have a pointer
      free(temp->pointer);
    free(temp);
  }
  if(curr != NULL){
    free(curr->pointer->p);
    free(curr->pointer);
    free(curr);

  }
  head = NULL;
}

// Replacement for the C function "exit", that consists of two lines: it calls
// tfree before calling exit. It's useful to have later on; if an error happens,
// you can exit your program, and all memory is automatically cleaned up.
void texit(int status){
  tfree();
  exit(status);
}
