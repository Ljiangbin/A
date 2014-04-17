#ifndef STACK_H_
#define STACK_H_

#include "Set.h"

typedef struct LinkedList StackNode, *Stack;

struct LinkedList
{
	Set set;
	Stack next;
};

Stack CreateStack();
void DeleteStack(Stack s);
void Push(Stack stack, Set set);
void Pop(Stack s);
Set Top(Stack s);
Bool IsEmpty(Stack s);

#endif //STACK_H_
