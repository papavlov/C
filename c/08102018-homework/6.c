//------------------------------------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: 6.c (unix file name)
// FILE PURPOSE:
// to implement a stack
// ...
//------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
typedef struct
{
    size_t size;
    size_t capacity;
    int *elements;
}stack;

void stack_init(stack *);
void stack_destroy(stack *);
int stack_empty(stack *);
void stack_push(stack *, int);
int stack_top(stack *);
void stack_pop(stack *);

void stack_init(stack *ptr){
	ptr->size = 0;
	ptr->capacity = 20;
	ptr->elements = (int*)malloc(sizeof(ptr->capacity));
	
}

//------------------------------------------------------------------------
// FUNCTION: stack_init
// to initialize elements of the stack
// PARAMETERS:
// *ptr is a pointer to the structure stack
//------------------------------------------------------------------------

void stack_destroy(stack *ptr){

	free(ptr->elements);
	ptr->elements = 0;
	ptr->size = 0;
	ptr->capacity = 0;	
	
}

//------------------------------------------------------------------------
// FUNCTION: stack_destroy
// to deinitialize the elements of the stack
// PARAMETERS:
// *ptr is a pointer to the structure
//------------------------------------------------------------------------

int stack_empty(stack *ptr){
	if(ptr->size==0){
		return 1;
	}
	else{
		
		return 0;

	}
}

//------------------------------------------------------------------------
// FUNCTION: stack_empty
// to check wether the stack is empty or not.If the stack is not empty, the function should return 0.If the stack is empty, the function should return a result different from 0
// PARAMETERS:
// *ptr is a pointer to the structure stack
//------------------------------------------------------------------------

void stack_push(stack *ptr, int a){
	if(ptr->size <= ptr->capacity){
		ptr->elements[ptr->size] = a;
		printf("ELement added: element = %d\n", ptr->elements[ptr->size]);
		ptr->size++;
	}
	

}

//------------------------------------------------------------------------
// FUNCTION: stack_push
// adds an element to the stack
// PARAMETERS:
// *ptr is a pointer to the structure stack
// a is a variable, that will be added to the stack
//------------------------------------------------------------------------

int stack_top(stack *ptr){
	return ptr->elements[ptr->size-1];
}

//------------------------------------------------------------------------
// FUNCTION: stack_top
// returns the last added element to the stack
// PARAMETERS:
// *ptr is a pointer to the structure stack
//------------------------------------------------------------------------

void stack_pop(stack *ptr){
	if(stack_empty(ptr)==0){
		ptr->size--;
		printf("Popped element = %d\n", ptr->elements[ptr->size]);
	}
	else{
		printf("There are no elements to pop because the stack is empty\n");

	}
	
}

//------------------------------------------------------------------------
// FUNCTION: stack_pop
// returns the last added element to the stack
// PARAMETERS:
// *ptr is a pointer to the structure stack
//------------------------------------------------------------------------

int main(){
	stack *s1 = malloc(sizeof(stack));
	int a = 10;
	int b = 3;
	int c = 7;
	stack_init(s1);
	stack_push(s1, a);
	stack_push(s1, b);
	stack_push(s1, c);
	stack_top(s1);
	stack_pop(s1);
	stack_pop(s1);
	stack_pop(s1);
	stack_destroy(s1);
	free(s1);	
	return 0;
}
