#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "Set.h"

typedef struct connode 
{
	Set data;
	Link LocationCur,LocationCon;
	struct connode *next;
}*ConflictLink,ConflictNode;
