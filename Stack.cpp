#include "Set.h"
#include "Stack.h"

Stack CreateStack()
{
	return (Stack)calloc(1, sizeof(StackNode));
}

void DeleteStack(Stack s)
{
	Stack p = NULL;

	while (s != NULL)
	{
		p = s->next;
		free(s);
		s = p;
	}
}

void Push(Stack stack, Set set)
{
	if (stack != NULL)
	{
		Stack p = stack->next;
		stack->next = CreateStack();
		stack->next->set = set;
		stack->next->next = p;
	}
}

void Pop(Stack s)
{
	if (!IsEmpty(s))
	{
		Stack p = s->next;
		s->next = p->next;
		free(p);
	}
}

Set Top(Stack s)
{
	Set set = NULL;

	if (!IsEmpty(s))
		set = s->next->set;

	return set;
}

Bool IsEmpty(Stack s)
{
	return (s == NULL || s->next == NULL);
}
