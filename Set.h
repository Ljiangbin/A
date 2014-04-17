#ifndef SET_H_
#define SET_H_
#define SEGMENTTREE_IMPLT

#include <stdlib.h>

#define False	0
#define True	1

typedef unsigned int Address;
typedef char Bool, Byte;
typedef struct BiTree SetNode, *Set;

struct BiTree
{
	Address front;
	Address rear;
	SetNode *left;
	SetNode *right;
	Byte height;
};

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// A set must be initialized by CreateSet.
Set CreateSet();

// Insert an address addr of length size into set s, then return the set.
Set Insert(Set s, Address addr, size_t size);

// Search the address addr in set s. If addr exists, return True, otherwise return False.
Bool LookUp(Set s, Address addr);

// Return the union of set a and b.
Set Union(Set a, Set b);

// Return the intersection of set a and b.
Set Intersection(Set a, Set b);

// Print out all the addresses in set s to stdout.
void PrintSet(Set s);

// Delete the set s.
void DeleteSet(Set s);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //SET_H_
