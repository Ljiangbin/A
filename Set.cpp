#include "Set.h"
#include "Stack.h"
#include <stdio.h>

#define Max(a, b) (((a) > (b)) ? (a) : (b))
#define Min(a, b) (((a) < (b)) ? (a) : (b))
#define Height(s) (((s) == NULL) ? -1 : (s)->height)
#define IsLeaf(s) (((s)->left == NULL) ? True : False)

static Set CreateSetNode(Address front, Address rear);
static Stack FindAllLeaves(Stack stack, Set set);
static void PostOrder(Stack stack, Set set);
static Stack UnionFromStack(Stack stackA, Stack stackB, Stack stack);
static Set FindSubtree(Set s, Address front, Address rear);
static Stack IntersectFromStack(Stack stackA, Stack stackB, Stack stack);
static void AddSetNodeToStack(Stack stack, Set set);
static Set BuildTree(Stack stackOne);
static Set getSmallOne(Set A, Set B);
static Set IsCoverd(Set A, Set B);
static Set Rotate(Set s);
static Set SingleLeft(Set s);
static Set SingleRight(Set s);
static Set DoubleLeft(Set s);
static Set DoubleRight(Set s);

// A set must be initialized by CreateSet.
Set CreateSet()
{
	return NULL;
}

// Insert an address addr of length size into set s, then return the set.
Set Insert(Set s, Address addr, size_t size)
{
	Bool mergeFlag = True;

	if (s == NULL)
	{
		s = CreateSetNode(addr, addr + size - 1);
	}
	else
	{
		/* insert */
		if ((addr + size == s->front)||(addr + size - 1 == s->front))
		{
			if (!IsLeaf(s))
				s->left = Insert(s->left, addr, size);
			else
				mergeFlag = False;
			s->front -= size;
		}
		else if ((addr == s->rear + 1)||(addr == s->rear))
		{
			if (!IsLeaf(s))
				s->right = Insert(s->right, addr, size);
			else
				mergeFlag = False;
			s->rear += size;
		}
		else if (addr + size < s->front)
		{
			if (IsLeaf(s))
			{
				s->right = CreateSetNode(s->front, s->rear);
				s->left = CreateSetNode(addr, addr + size - 1);
				s->front = addr;
			}
			else
			{
				s->left = Insert(s->left, addr, size);
				s->front = addr;
				s = Rotate(s);
			}
		}
		else if (addr > s->rear + 1)
		{
			if (IsLeaf(s))
			{
				s->left = CreateSetNode(s->front, s->rear);
				s->right = CreateSetNode(addr, addr + size - 1);
				s->rear = addr + size - 1;
			}
			else
			{
				s->right = Insert(s->right, addr, size);
				s->rear = addr + size - 1;
				s = Rotate(s);
			}
		}
		else if (addr <= s->front && addr + size - 1 >= s->rear)
		{
			s->front = addr;
			s->rear = addr + size - 1;
			free(s->left);
			free(s->right);
			s->left = s->right = NULL;
			s = Rotate(s);
			//return s;
		}
		else
		{
			if (IsLeaf(s)){
			/*	�޸Ĺ���	*/
			    s->front = Min(s->front,addr);
				s->rear  = Max(s->rear,addr + size - 1);
				return s;
				}

			if (addr + size - 1 > s->rear)
			{
				//s->right = Insert(s->right, addr, size);
				//s->rear = addr + size - 1;
				//s->right = Insert(s->right, s->rear, addr + size - s->rear);
				Address tmp = s->rear;
				s = Insert(s, s->rear+1, addr + size - s->rear - 1);
				s = Insert(s, addr,tmp - addr + 1);
				s = Rotate(s);
			}
			else if(addr < s->front)
			{
				//s->front = addr;
				//s->left = Insert(s->left, addr, s->front - addr + 1);
				Address tmp = s->front;
				s = Insert(s, addr, s->front - addr );
				s = Insert(s, tmp, addr + size - tmp );
				//s->left = Insert(s->left, addr, size);
				s = Rotate(s);
			}
			else
			{
				if(addr >= s->right->front){
				   s->right = Insert(s->right, addr, size);
				}else if(addr + size - 1 <= s->right->front){
					s->left = Insert(s->left, addr, size);
				}else {
					s->left = Insert(s->left, addr, s->right->front - addr + 1);
					s->right = Insert(s->right, s->right->front, addr + size - s->right->front);
				}
			}
		}
		/* insert */

		/* merge */
		if ((s->left != NULL && s->right != NULL)&&mergeFlag &&( (s->left->rear >= s->right->front)||(s->left->rear+1 >= s->right->front) )&& IsLeaf(s->left) && IsLeaf(s->right))
		{
		//	printf("a\n");
			free(s->left);
			free(s->right);
			s->left = s->right = NULL;
		}
		/* merge */

		/* update height */
		s->height = Max(Height(s->left), Height(s->right)) + 1;
		/* update height */
	}

	return s;
}

// Search the address addr in set s. If addr exists, return True, otherwise return False.
Bool LookUp(Set s, Address addr)
{
	if (s == NULL || addr < s->front || addr > s->rear)
		return False;

	if (IsLeaf(s))
		return True;

	if (addr <= s->left->rear)
		return LookUp(s->left, addr);

	if (addr >= s->right->front)
		return LookUp(s->right, addr);

	return False;
}

// Return the union of set a and b.
Set Union(Set A, Set B)
{
	Stack stackA = NULL, stackB = NULL, stack = NULL;
	Set unionedSet = NULL;

	if (A == NULL)
	{
		if (B != NULL)
		{
			stackB = FindAllLeaves(CreateStack(), B);
			unionedSet = BuildTree(stackB);
			DeleteStack(stackB);
		}
	}
	else
	{
		if (B == NULL)
		{
			stackA = FindAllLeaves(CreateStack(), A);
			unionedSet = BuildTree(stackA);
			DeleteStack(stackA);
		}
		else
		{
			// �ҵ�����Ҷ�ӽڵ�
			stackA = FindAllLeaves(CreateStack(), A);
			stackB = FindAllLeaves(CreateStack(), B);

			// ���Ե��󲢼�
			stack = UnionFromStack(stackA, stackB, CreateStack());
			DeleteStack(stackA);
			DeleteStack(stackB);

			// ��ջ����������
			unionedSet = BuildTree(stack);
			DeleteStack(stack);
		}
	}
	return unionedSet;
}

// Return the intersection of set a and b.
Set Intersection(Set A, Set B)
{
	Address front = 0, rear = 0;
	Stack stack = NULL, stackA = NULL, stackB = NULL;
	Set intersectedSet = NULL, partA = NULL, partB = NULL;

	if (A == NULL || B == NULL || A->rear < B->front || B->rear < A->front)
		return NULL;

	front = Max(A->front, B->front);
	rear = Min(A->rear, B->rear);

	// �ҵ������н�����Ҷ�ӽڵ�
	partA = FindSubtree(A, front, rear);
	partB = FindSubtree(B, front, rear);
	stackA = FindAllLeaves(CreateStack(), partA);
	stackB = FindAllLeaves(CreateStack(), partB);

	// ���Ե��󲢼�
	stack = IntersectFromStack(stackA, stackB, CreateStack());
	DeleteStack(stackA);
	DeleteStack(stackB);

	// ��ջ����������
	intersectedSet = BuildTree(stack);
	DeleteStack(stack);
	return intersectedSet;
}

// Delete the set s.
void DeleteSet(Set s)
{
	if (s != NULL)
	{
		DeleteSet(s->left);
		DeleteSet(s->right);
		free(s);
	}
}

/*void PrintSet2(Set s)
{
 
  // printf("\nStart-------------\n");
	if (s != NULL)
	{
		if (IsLeaf(s))
			fprintf(test,"0x%08x 0x%08x\n", s->front, s->rear);
			//printf("0x%08x-0x%08x ", s->front, s->rear);
		else
		{
			PrintSet(s->left);
			PrintSet(s->right);
		}
	}
//	printf("\nEnd-------------\n");
//	fclose(test);
}*/
// ���������е�һ�����
static Set CreateSetNode(Address front, Address rear)
{
	if(rear < front){
		printf("front = %x, rear = %x\n", front, rear);
		exit(0);
	}
	Set set = (Set)malloc(sizeof(SetNode));
	set->front = front;
	set->rear = rear;
	set->left = NULL;
	set->right = NULL;
	set->height = 0;
	return set;
}

// �ҵ�����front ��rear ֮���ֵ������Ҷ�ӽڵ㣬��ջ�ṹ����
// ���������ʽ��ʱ��O(n)
static Stack FindAllLeaves(Stack stack, Set set)
{
	PostOrder(stack, set);
	return stack;
}

// ����������Ƚ�������������֤��ջʱΪ��С�����˳��
static void PostOrder(Stack stack, Set set)
{
	if (set != NULL)
	{
		PostOrder(stack, set->right);
		PostOrder(stack, set->left);
		if (IsLeaf(set))
			Push(stack, set);
	}
}

// ���ü���A������B ����Ҷ���γɵ�ջstacka��stackb �󲢼����������ջ
static Stack UnionFromStack(Stack stackA, Stack stackB, Stack stack)
{
	Set A = Top(stackA);
	Set B = Top(stackB);
	while (A != NULL && B != NULL)
	{
		Set coveredNode = IsCoverd(A, B);
		// A ������ �� B ������
		if (coveredNode != NULL)
		{
			// �����������ģ�ֱ������ƶ�һ��
			if (coveredNode == A)
			{
				Pop(stackA);
				A = Top(stackA);
			}
			else
			{
				Pop(stackB);
				B = Top(stackB);
			}
		}
		// �����������������������С�ļ���ջ���ҽ�С������ƶ�һ��
		else
		{
			Set smallOne = getSmallOne(A, B);
			AddSetNodeToStack(stack, smallOne);
			if (smallOne == A)
			{
				Pop(stackA);
				A = Top(stackA);
			}
			else
			{
				Pop(stackB);
				B = Top(stackB);
			}
		}
	}
	while (A != NULL)
	{
		AddSetNodeToStack(stack, A);
		Pop(stackA);
		A = Top(stackA);
	}
	while (B != NULL)
	{
		AddSetNodeToStack(stack, B);
		Pop(stackB);
		B = Top(stackB);
	}
	return stack;
}

// Find the minimum subtree of set s that covers addresses from front to rear.
static Set FindSubtree(Set s, Address front, Address rear)
{
	if(s->front <= front && s->rear >= rear) return s;
	if (s->left->rear >= rear)
	{
		if (IsLeaf(s->left))
			return s->left;
		else
			return FindSubtree(s->left, front, rear);
	}
	if (s->right->front <= front)
	{
		if (IsLeaf(s->right))
			return s->right;
		else
			return FindSubtree(s->right, front, rear);
	}
	return s;
}

static Stack IntersectFromStack(Stack stackA, Stack stackB, Stack stack)
{
	Set A = Top(stackA);
	Set B = Top(stackB);
	while (A != NULL && B != NULL)
	{
		Set coveredNode = IsCoverd(A, B);
		// A �������� B ������,���������Ľ�����ջ���������һλ
		if (coveredNode != NULL)
		{
			Push(stack, CreateSetNode(coveredNode->front, coveredNode->rear));
			if (coveredNode == A)
			{
				Pop(stackA);
				A = Top(stackA);
			}
			else
			{
				Pop(stackB);
				B = Top(stackB);
			}
		}
		// �����������򽫽�С������ƶ�һλ
		else if (A->rear < B->front || B->rear < A->front)
		{
			Set smallNode = getSmallOne(A, B);
			if (smallNode == A)
			{
				Pop(stackA);
				A = Top(stackA);
			}
			else
			{
				Pop(stackB);
				B = Top(stackB);
			}
		}
		// �����������غϲ��ּ���ջ��������С������ƶ�һλ
		else
		{
			Set smallNode = getSmallOne(A, B);
			Set bigNode;
			if (smallNode == A)
			{
				bigNode = B;
				Pop(stackA);
				A = Top(stackA);
			}
			else
			{
				bigNode = A;
				Pop(stackB);
				B = Top(stackB);
			}
			Push(stack, CreateSetNode(bigNode->front, smallNode->rear));
		}
	}
	return stack;
}

// ���A��B����н�Ϊ��ǰ���Ǹ�
static Set getSmallOne(Set A, Set B)
{
	if (B->front < A->front)
		return B;
	else if (A->front < B->front)
		return A;
	else
		return NULL;
}

// �жϽ�� A �Ƿ������� B �� ��� B �Ƿ������� A
// ���ر������Ľ�㣬������ΪNULL
static Set IsCoverd(Set A, Set B)
{
	// A ���� B
	if (B->front >= A->front && B->rear <= A->rear)
		return B;
	else if (A->front >= B->front && A->rear <= B->rear)
		return A;
	else
		return NULL;
}

// ��һ�����ϵĽ�����ջ��
static void AddSetNodeToStack(Stack stack, Set set)
{
	Set top = Top(stack);
	// �����Ҫ����Ľ���뵱ǰջ�������������ֻ���޸�ֵ
	if (top != NULL && top->rear + 1 >= set->front)
	{
		top->rear = set->rear;
	}
	else
	{
		Push(stack, CreateSetNode(set->front, set->rear));
	}
}

// Build a set from an ordered leaf sequence in a stack.
static Set BuildTree(Stack stackOne)
{
	Stack stackTwo = CreateStack();
	Stack *currentStack = &stackOne;
	Stack *nextStack = &stackTwo;
	Set set = CreateSet();
	int height = 1;

	while (!IsEmpty(*currentStack) && (*currentStack)->next->next != NULL)
	{
		while (!IsEmpty(*currentStack))
		{
			Set a = Top(*currentStack);
			Pop(*currentStack);
			Set b = NULL;
			if (!IsEmpty(*currentStack))
			{
				Set c = Top(*currentStack);
				Pop(*currentStack);
				b = (Set)malloc(sizeof(SetNode));
				b->front = Min(a->front, c->front);
				b->rear = Max(a->rear, c->rear);
				b->left = a->front < c->front ? a : c;
				b->right = a->front < c->front ? c : a;
			}
			else
				b = a;

			b->height = height;
			Push(*nextStack, b);
		}
		Stack *tmp = currentStack;
		currentStack = nextStack;
		nextStack = tmp;
		++height;
	}
	set = Top(*currentStack);

	DeleteStack(stackTwo);
	return set;
}

static Set Rotate(Set s)
{
	if (Height(s->left) - Height(s->right) == 2)
	{
		if (Height(s->left->left) > Height(s->left->right))
			return SingleLeft(s);
		else
			return DoubleLeft(s);
	}
	else if (Height(s->right) - Height(s->left) == 2)
	{
		if (Height(s->right->right) > Height(s->right->left))
			return SingleRight(s);
		else
			return DoubleRight(s);
	}
	else
		return s;
}

static Set SingleLeft(Set s)
{
	Set ls = s->left;

	s->left = ls->right;
	ls->right = s;
	s->front = s->left->front;
	ls->rear = s->rear;
	s->height = Max(Height(s->left), Height(s->right)) + 1;
	ls->height = Max(Height(ls->left), s->height) + 1;
    if(ls->rear < ls->front) printf("ls rear %d front %d\n",ls->rear,ls->front);
	return ls;
}

static Set SingleRight(Set s)
{
	Set rs = s->right;

	s->right = rs->left;
	rs->left = s;
	s->rear = s->right->rear;
	rs->front = s->front;
	s->height = Max(Height(s->left), Height(s->right)) + 1;
	rs->height = Max(s->height, Height(rs->right)) + 1;
	if(rs->rear < rs->front) printf("rs rear %d front %d\n",rs->rear,rs->front);
	return rs;
}

static Set DoubleLeft(Set s)
{
	Set ls = s->left;
	Set lrs = ls->right;

	ls->right = lrs->left;
	s->left = lrs->right;
	lrs->left = ls;
	lrs->right = s;
	ls->rear = ls->right->rear;
	s->front = s->left->front;
	lrs->front = ls->front;
	lrs->rear = s->rear;
	ls->height = Max(Height(ls->left), Height(ls->right)) + 1;
	s->height = Max(Height(s->left), Height(s->right)) + 1;
	lrs->height = Max(ls->height, s->height) + 1;
     if(lrs->rear < lrs->front) printf("lrs rear %d front %d\n",lrs->rear,lrs->front);
	return lrs;
}

static Set DoubleRight(Set s)
{
	Set rs = s->right;
	Set rls = rs->left;

	s->right = rls->left;
	rs->left = rls->right;
	rls->left = s;
	rls->right = rs;
	s->rear = s->right->rear;
	rs->front = rs->left->front;
	rls->front = s->front;
	rls->rear = rs->rear;
	s->height = Max(Height(s->left), Height(s->right)) + 1;
	rs->height = Max(Height(rs->left), Height(rs->right)) + 1;
	rls->height = Max(s->height, rs->height) + 1;
    if(rls->rear < rls->front) printf("rls rear %d front %d\n",rls->rear,rls->front);
	return rls;
}

