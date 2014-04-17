#include<stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "Set.h"

#define	TRUE	1
#define	FALSE	0
#define	OK	1
#define	ERROR	0
#define	INFEASIBLE	-1
#define	OVERFLOW	-2   
#define ThreadNum	4096

typedef struct{
	Set	MemReadSet;
	Set	MemWriteSet;
	int	clock[ThreadNum];
}Sets;

typedef struct LNode{
	Sets	data;
	LNode*	next;
}*Link;

typedef struct{
	Link	head,tail;
	int	len;
}LinkSet;


int InitSets(LinkSet &ls){   
	//初始化 集合
	ls.head = (Link) malloc( sizeof(LNode));
	ls.head->data.MemReadSet = CreateSet();
	ls.head->data.MemWriteSet = CreateSet();
	for(int i=0;i<ThreadNum;i++)
	{
		ls.head->data.clock[i] = 0;
	}
	if(!ls.head) exit(OVERFLOW);    //如果分配失败

	ls.head->next = NULL;        //头、尾指针为空
	ls.len = 0;                                    //长度为0
	return OK;
}

int CreateNode(Link &link){
	//创建一节点
	link = (Link) malloc( sizeof(LNode));
	if(!link)    exit(OVERFLOW);
	link->data.MemReadSet = CreateSet();
	link->data.MemWriteSet = CreateSet();
	for(int i=0;i<ThreadNum;i++)
	{
		link->data.clock[i] = 0;
	}
	link->next = NULL;                            //指向空
	return OK;
}

int InsertNode(LinkSet &ls, Link &link){
	//向链表头部插入节点
	if(ls.head->next != NULL)
	{
		for(int i=0;i<ThreadNum;i++)
		{
			link->data.clock[i] = ls.head->next->data.clock[i];
		}
	}
	else
	{
		for(int i=0;i<ThreadNum;i++)
		{
			link->data.clock[i] = 0;
		}
	}
	link->next = ls.head->next;
	ls.head->next = link;
	ls.len ++;
	return OK;
}

