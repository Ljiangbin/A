#include<stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define ThreadNum       4096

typedef struct node 
{
  unsigned long LockID;
  int	clock[ThreadNum];
  struct node *next;
}*LockLink,LockNode;

LockLink inilink()
{
  LockLink head;
  head=(LockLink)malloc(sizeof(struct node));
  head->next=NULL;
  for(int i=0;i<ThreadNum;i++)
  {
	head->clock[i] = 0;
  }
  return head;
}
