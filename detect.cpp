#include <stdio.h>
#include "pin.H"
#include "LinkSet.h"
#include "LockSet.h"
#include "ConflictSet.h"
#include <iostream>

using namespace std;
#define IsLeaf(s) (((s)->left == NULL) ? True : False)
#define ThreadNum	4096
#define PIN_PRINT(x)	cerr << x << endl;
#ifdef _WIN32
#define PIN_GetLock GetLock
#define PIN_ReleaseLock ReleaseLock
#define PIN_InitLock InitLock
#define MutexInit "CreateMutexA"
#define MutexDestroy "CloseHandle"
#define MutexLock "WaitForSingleObject"
#define MutexUnlock "ReleaseMutex"
#define OK 1
#else
#define MutexInit "pthread_mutex_init"
#define MutexDestroy "pthread_mutex_destroy"
#define MutexLock "pthread_mutex_lock"
#define MutexUnlock "pthread_mutex_unlock"
#define OK 0
#endif

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "data.out", "specify output file name");

PIN_LOCK lock;
static UINT64 tcount = 0;
LinkSet RecordData[ThreadNum];
LockLink LL;
ConflictLink ConflictSet;
int globalclock[ThreadNum][ThreadNum] = {0};
FILE *file;
FILE *test;
void printVC();
/*
void PrintSet(Set s, int ident = 0)
{
 
  // printf("\nStart-------------\n");
	if (s != NULL)
	{
		for(int i = 0; i < ident * 2; i++){
			fprintf(test," ");
		}
		fprintf(test,"0x%08x 0x%08x\n", s->front, s->rear);
		if(s->rear < s->front){
				fprintf(test,"->>>>>>>>>>>>\n");
		}
		if (IsLeaf(s)){}
			//fprintf(test,"0x%08x 0x%08x\n", s->front, s->rear);
			//if(s->rear < s->front){
			//	fprintf(test,"->>>>>>>>>>>>\n");
			//}
			//printf("0x%08x-0x%08x ", s->front, s->rear);
		else
		{
			PrintSet(s->left, ident + 1);
			PrintSet(s->right, ident + 1);
		}
	}
//	printf("\nEnd-------------\n");
//	fclose(test);
}*/
void PrintSet(Set s)
{
	if (s != NULL)
	{
		if (IsLeaf(s)){
			fprintf(test,"0x%08x-0x%08x\n", s->front, s->rear);
			if(s->rear < s->front){
				fprintf(test,"->>>>>>>>>>>>\n");
			}
			}
		else
		{
			PrintSet(s->left);
			PrintSet(s->right);
		}
	}
}
// Note that opening a file in a callback is only supported on Linux systems.
// See buffer-win.cpp for how to work around this issue on Windows.
//
// This routine is executed every time a thread is created.


VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v)
{
PIN_PRINT("ThreadStartB");
	PIN_GetLock(&lock, threadid+1);
	tcount++;
	// Initialize the threads LinkSet
	InitSets(RecordData[threadid]);
	Link NewNode;
	CreateNode(NewNode);
	InsertNode(RecordData[threadid],NewNode);
	//RecordData[threadid].head->next->data.clock[threadid]++;
	//printf("threadid=%d, p=%p\n", threadid, RecordData[threadid].head->next);
	PIN_ReleaseLock(&lock);
PIN_PRINT("ThreadStartE");
}

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v)
{
PIN_PRINT("ThreadFiniB");
	PIN_GetLock(&lock, threadid+1);
	Link NewNode;
	CreateNode(NewNode);
	InsertNode(RecordData[threadid],NewNode);
	RecordData[threadid].head->next->data.clock[threadid]++;
	PIN_ReleaseLock(&lock);
PIN_PRINT("ThreadFiniE");
}

// This routine is executed each time pthread_mutex_init is called.
VOID BeforeInitLock(ADDRINT getlock, THREADID threadid)
{
PIN_PRINT("BeforeInitLockB");
//PIN_PRINT(getlock);
printf("BeforeInitLock %d\n",getlock);
	PIN_GetLock(&lock, threadid+1);
	fprintf(file,"BeforeInitLockB-----\n");
	printVC();
	LockLink p;
	p = (LockLink)malloc(sizeof(struct node));
	p->LockID = (unsigned long)getlock;
	if(LL->next == NULL)
	{
		LL->next = p;
		p->next = NULL;
	}	
	else
	{
		p->next = LL->next;
		LL->next = p;
	}
	//free(p);
//PIN_PRINT(LL->next->LockID);
    //LockLink q = LL->next;
   /* fprintf(file,"%s","\n-----------------\nBeforeInitLock    ");
	while(q != NULL)
    {
	  fprintf(file,"%d ",q->LockID);
	  q = q->next;
	}
	fprintf(file,"%s","\n-----------------\n");*/
	fprintf(file,"BeforeInitLockE-----\n");
	printVC();
	PIN_ReleaseLock(&lock);
PIN_PRINT("BeforeInitLockE");
}

// This routine is executed each time pthread_mutex_destroy is called.
VOID BeforeDestroyLock(ADDRINT getlock, THREADID threadid,ADDRINT ReVal)
{
if(!ReVal) {
	printf("DestroyLock Fail\n");
	return;
}
PIN_PRINT("BeforeDestroyLockB");
printf("BeforeDestroyLock %d\n",getlock);
	PIN_GetLock(&lock, threadid+1);
	fprintf(file,"BeforeDestroyLockB-----\n");
	printVC();
	LockLink p,q;
	p = LL;
	q = LL->next;
	while(q != NULL)
	{
		if(q->LockID == (unsigned long)getlock)
		{
			p->next = q->next;
			free(q);
			break;
		}
		p = p->next;
		q = q->next;
	}
	/*LockLink q1 = LL->next;
	fprintf(file,"%s","\n-----------------\BeforeDestroyLock    ");
	while(q1 != NULL)
    {
	  fprintf(file,"%d ",q1->LockID);
	  q1 = q1->next;
	}
	fprintf(file,"%s","\n-----------------\n");*/
	fprintf(file,"BeforeDestroyLockE-----\n");
	printVC();
	PIN_ReleaseLock(&lock);
PIN_PRINT("BeforeDestroyLockE");
}

void printVC(){
	int j = (int)tcount;
	fprintf(file,"---------------------------------------------------------\n");
	fprintf(file,"total thread = %lld\n", tcount);
	//print globalclock
	for(int m = 0; m < j; m++)
	{
		for(int n = 0; n < j; n++)
		{
			fprintf(file,"globalclock[%d][%d] = %d\n",m,n,globalclock[m][n]);
		}
	}

	//print RecordData's clock
	Link printPointer;
	for(int m = 0; m < j; m++)
	{	
		printPointer = RecordData[m].head->next;
		//printf("%p\n",printPointer);
		for(int n = 0; n < j; n++)
		{
		
			fprintf(file,"RecordData[%d][%d]->data = %d\n",m,n,printPointer->data.clock[n]);
		}
	}

	//print Lock's clock
	LockLink p;
	p = LL;
	while(p != NULL)
	{
		for(int i = 0; i < j; i++)
		{
			fprintf(file,"Lock ID = %ld,Lock clock[%d] = %d\n",p->LockID,i,p->clock[i]);
		}
		p = p->next;
	}
}

// This routine is executed each time pthread_mutex_lock is called.
VOID BeforeGetLock( THREADID threadid,ADDRINT getlock ,ADDRINT ReVal)
{
    if(ReVal != 0)
	{
	   printf("HAHAHAHAHA!\n");
	   return;
	}
	PIN_PRINT("BeforeGetLockB");
	printf("id = %d BeforeGetLock %d\n",threadid,getlock);
	PIN_GetLock(&lock, threadid+1);
	//printf("thread %d getlock\n", threadid);
	fprintf(file,"BeforeGetLockB-----\n");
	printVC();
	//Fix the Lock's location.
	unsigned long CurLockID = getlock;
	LockLink	Lock;
	Lock = LL->next;
	LockLink lckTemp = NULL;
	//printf("lock=%d\n", getlock);

	while(Lock != NULL)
	{
		//printf("lock2=%ld\n", Lock->LockID);
		if(CurLockID == Lock->LockID){
			lckTemp = Lock;
			break;
		}
		else
			Lock = Lock->next;
	}

	if(lckTemp != NULL){

		//Detect conflict
		int j = (int)tcount;
		fprintf(file,"\n------------------BeforeGetLock  threadid = %d\n",threadid);
		for(int k = 0; k < j; k++)
		{
			if(k == (int)threadid)
				continue;

			int myLower = globalclock[threadid][k];
			int myUpper = RecordData[threadid].head->next->data.clock[threadid];
			int itsLower = RecordData[threadid].head->next->data.clock[k];
			int itsUpper = Lock->clock[k];
			fprintf(file,"mylower=%d, myupper=%d, itslower=%d, itsupper=%d\n", myLower,myUpper, itsLower, itsUpper);
			//printf("i'm here");
			//fflush(stdout);
			Link myUpperPointer = NULL;
			Link itsUpperPointer = NULL;
			  //Current thread's pointer
			myUpperPointer = RecordData[threadid].head->next;
			   //Contrast thread's pointer
			itsUpperPointer = RecordData[k].head->next;
			/*fprintf(file,"\n myUpperPointer  ");
			while(myUpperPointer != NULL)
			{
			  fprintf(file,"%d  ",myUpperPointer->data.clock[threadid]);
			  myUpperPointer = myUpperPointer->next;
			}
			fprintf(file,"\n itsUpperPointer  ");
			while(itsUpperPointer != NULL)
			{
			  fprintf(file,"%d ",itsUpperPointer->data.clock[k]);
			  itsUpperPointer = itsUpperPointer->next;
			}
			fprintf(file,"\n");*/
			if(myUpper < myLower || itsUpper <= itsLower){
				continue;
			}
			//while(CurPointer->data.clock[threadid] > Lock->clock[threadid])
			//	CurPointer = CurPointer->next;
			//printf("mylower=%d, myupper=%d, itslower=%d, itsupper=%d\n", myLower,myUpper, itsLower, itsUpper);
			while(itsUpperPointer->data.clock[k] >= itsUpper && itsUpperPointer != NULL)
				{
					//printf("it %d %d\n",itsUpperPointer->data.clock[k],itsUpper);
					itsUpperPointer = itsUpperPointer->next;
				}
			while(myUpperPointer->data.clock[threadid] > myUpper &&  myUpperPointer != NULL)
				{
					//printf("my %d %d\n",myUpperPointer->data.clock[threadid],itsUpper);
					myUpperPointer = myUpperPointer->next;
				}
			if(itsUpperPointer == NULL || myUpperPointer == NULL)
				continue;
			
			
			Link myTemp = NULL;
			Link itsTemp = NULL;

			myTemp = myUpperPointer;
			for(int i = myUpper; i >= myLower ; i--){
				itsTemp = itsUpperPointer;
				//printf("i = %d\n",i);
				//fprintf(test,"myTemp->data.MemReadSet %d\n",i);
			//	PrintSet(myTemp->data.MemReadSet);
			//	fprintf(test,"myTemp->data.MemWriteSet %d\n",i);
			//	PrintSet(myTemp->data.MemWriteSet);
				for(int j = itsUpper - 1; j >= itsLower ; j--){
				            //Intersection(myUpperPointer->data.MemWriteSet,itsUpperPointer->data.MemWriteSet);
				//fprintf(test,"itsTemp->data.MemReadSet %d\n",j);
				//PrintSet(itsTemp->data.MemReadSet);
			  //  fprintf(test,"itsTemp->data.MemWriteSet %d\n",j);
			 //    PrintSet(itsTemp->data.MemWriteSet);
				//	printf(" compare my=%d, its=%d\n", i, j);
				//	fflush(stdout);
					
				//	if(myTemp == NULL) printf("NULL\n");
				//	if(itsTemp == NULL) printf("NULL1\n");
					Set RandW = Intersection(itsTemp->data.MemReadSet,myTemp->data.MemWriteSet);
					Set WandR = Intersection(myTemp->data.MemWriteSet,itsTemp->data.MemReadSet);
					Set WandW = Intersection(myTemp->data.MemWriteSet,itsTemp->data.MemWriteSet);
				//	printf("mm\n");
					//printf("RandW %d %d \n",RandW->front,RandW->rear);
					//printf("WandR %d %d \n",WandR->front,WandR->rear);
					//printf("WandW %d %d \n",WandW->front,WandW->rear);
					//if(RandW == NULL) printf("RandW is NULL\n");
				//	if(WandR == NULL) printf("WandR is NULL\n");
				//	if(WandW == NULL) printf("WandW is NULL\n");
					Set Conflict = Union(RandW,WandR);
					Conflict = Union(Conflict,WandW);
					if(Conflict != NULL)
					{
						//printf("data race found!\n");
					/*	fprintf(file,"data race found! my tid=%d, its tid = %d\n", threadid, k);
						
						fprintf(test, "its read\n");
						PrintSet(itsTemp->data.MemReadSet, 0);
						fprintf(test, "its write\n");
						PrintSet(itsTemp->data.MemWriteSet, 0);
						fprintf(test, "my read\n");
						PrintSet(myTemp->data.MemReadSet, 0);
						fprintf(test, "my write\n");
						PrintSet(myTemp->data.MemWriteSet, 0);
						*/
						fprintf(test,"Conflict\n");
						
						PrintSet(Conflict);
						if(ConflictSet == NULL)
						{
							ConflictSet = (ConflictLink)malloc(sizeof(ConflictLink));
							ConflictSet->data = CreateSet();
							ConflictSet->next = NULL;
						}
						ConflictLink p,q;
						p = ConflictSet;
						while(p->next != NULL)
						{
							p = p->next;
						}
						q = (ConflictLink)malloc(sizeof(struct connode));
						q->data = CreateSet();
						q->LocationCur = NULL;
						q->LocationCon = NULL;
						q->data = Union(q->data,Conflict);
						p->next = q;
						q->next = NULL;
					}
					
					itsTemp = itsTemp->next;
				//	printf("itsnext=%p\n", myTemp);
				//	fflush(stdout);	
				}
				//printf("mynext=%p\n", myTemp);
				//fflush(stdout);
				
				myTemp = myTemp->next;
				
			}
			//update globalclock
			globalclock[k][threadid] = Lock->clock[k];
			/*if((globalclock[k][threadid] < Lock->clock[threadid])&&(globalclock[threadid][k] < Lock->clock[k]))
			//Current thread's range from globalclock[k][threadid](closed interval) to Lock->clock[threadid](closed interval)
			//Contrast thread's range from globalclock[threadid][k](closed interval) to Lock->clock[k](open interval)
			{
			
				while(CurPointer->data.clock[threadid] > globalclock[k][threadid])
				{
				
					while(ConPointer->data.clock[k] > globalclock[threadid][k])
					{
						Set RandW,WandR,WandW,Conflict;
						RandW = Intersection(CurPointer->data.MemReadSet,ConPointer->data.MemWriteSet);
						WandR = Intersection(CurPointer->data.MemWriteSet,ConPointer->data.MemReadSet);
						WandW = Intersection(CurPointer->data.MemWriteSet,ConPointer->data.MemWriteSet);
						Conflict = Union(RandW,WandR);
						Conflict = Union(Conflict,WandW);
						if(Conflict != NULL)
						{
							if(ConflictSet == NULL)
							{
								ConflictSet = (ConflictLink)malloc(sizeof(ConflictLink));
								ConflictSet->data = CreateSet();
								ConflictSet->next = NULL;
							}
							ConflictLink p,q;
							p = ConflictSet;
							while(p->next != NULL)
							{
								p = p->next;
							}
							q = (ConflictLink)malloc(sizeof(struct connode));
							q->data = CreateSet();
							q->LocationCur =CurPointer;
							q->LocationCon =ConPointer;
							q->data = Union(q->data,Conflict);
							p->next = q;
							q->next = NULL;
						}
						ConPointer = ConPointer->next;
						
					}
					CurPointer = CurPointer->next;
				}
				//update globalclock
				globalclock[k][threadid] = Lock->clock[k];
			}*/
			
		}
	}
	//Creat new node and update clock.
//GET_RETURN:

	Link NewNode;
	CreateNode(NewNode);
	InsertNode(RecordData[threadid],NewNode);
	RecordData[threadid].head->next->data.clock[threadid]++;
   	fprintf(file,"BeforeGetLockE-----\n");
	printVC();
	PIN_ReleaseLock(&lock);
	PIN_PRINT("BeforeGetLockE");
}

// This routine is executed each time pthread_mutex_unlock is called.
VOID BeforeGetUnlock( THREADID threadid,ADDRINT getlock ,ADDRINT ReVal)
{
if(!ReVal) {
	printf("unlock Fail\n");
	return;
}
PIN_PRINT("BeforeGetUnlockB");
    printf("BeforeGetUnlock11 %d\n",getlock);
	PIN_GetLock(&lock, threadid+1);
	fprintf(file,"BeforeGetUnlockB-----\n");
	printVC();
	//Fix the Lock's location.
	unsigned long CurLockID = (unsigned long)getlock;
	LockLink	Lock;
	Lock = LL->next;
	while(Lock != NULL)
	{
		
		if(CurLockID == Lock->LockID)
		{
		//	printf("while");
			//Creat new node and update clock.
			Link NewNode;
			CreateNode(NewNode);
			InsertNode(RecordData[threadid],NewNode);
			RecordData[threadid].head->next->data.clock[threadid]++;

			//printf("RecordData[threadid].head->next->data.clock[0]=%d\n", RecordData[threadid].head->next->data.clock[0]);
			//Take the maximum
			for(int i=0;i<ThreadNum;i++)
			{
				
				if(RecordData[threadid].head->next->data.clock[i] > Lock->clock[i]){
					Lock->clock[i] = RecordData[threadid].head->next->data.clock[i];
					fprintf(file,"Lock->clock[%d]=%d\n", i,Lock->clock[i]);
				}
				else{
					RecordData[threadid].head->next->data.clock[i] = Lock->clock[i];
				}
			}
		break;
		}
		else
			Lock = Lock->next;
	}
	//printVC();
	fprintf(file,"BeforeGetUnlockE-----\n");
	printVC();
	PIN_ReleaseLock(&lock);
PIN_PRINT("BeforeGetUnlockE");
}

// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr, UINT32 MemReadSize, THREADID threadid )
{
    /*fprintf(file,"threadid %d read addr %d ",threadid,addr);
	int j = (int)tcount;
	for(int m = 0; m < j; m++)
	{	
		for(int n = 0; n < j; n++)
		{
		
			fprintf(file,"RecordData[%d][%d]->data = %d ",m,n,RecordData[m].head->next->data.clock[n]);
		}
		fprintf(file,"\n");
	}*/
	unsigned int Address = (unsigned int)addr;
	RecordData[threadid].head->next->data.MemReadSet = Insert(RecordData[threadid].head->next->data.MemReadSet, Address, MemReadSize);
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr, UINT32 MemWriteSize, THREADID threadid )
{
	/*fprintf(file,"threadid %d write addr %d\n",threadid,addr);
	int j = (int)tcount;
	for(int m = 0; m < j; m++)
	{
		for(int n = 0; n < j; n++)
		{
		
			fprintf(file,"RecordData[%d][%d]->data = %d ",m,n,RecordData[m].head->next->data.clock[n]);
		}
		fprintf(file,"\n");
	}*/
	unsigned int Address = (unsigned int)addr;
	RecordData[threadid].head->next->data.MemWriteSet = Insert(RecordData[threadid].head->next->data.MemWriteSet, Address, MemWriteSize);
}
/*
VOID AfterInitLock(ADDRINT getlock,THREADID threadid)
{
PIN_PRINT("AfterInitLockB");
//PIN_PRINT(getlock);
printf("AfterInitLock %d ID %d\n",getlock,threadid);
PIN_PRINT("AfterInitLockE");
}*/
//====================================================================
// Instrumentation Routines
//====================================================================

// This routine is executed for each image.
VOID ImageLoad(IMG img, VOID *)
{
	PIN_PRINT("ImageLoad0");
	RTN rtn0 = RTN_FindByName(img, MutexInit);
	if (RTN_Valid( rtn0 ))
	{
		RTN_Open(rtn0);
		if(OK) 		RTN_InsertCall(rtn0, IPOINT_AFTER, AFUNPTR(BeforeInitLock),
					   IARG_FUNCRET_EXITPOINT_VALUE ,IARG_THREAD_ID,IARG_END);
		else RTN_InsertCall(rtn0, IPOINT_BEFORE, AFUNPTR(BeforeInitLock),
					   IARG_FUNCARG_ENTRYPOINT_VALUE,0,
					   IARG_THREAD_ID, IARG_END);
				

					   			   
		RTN_Close(rtn0);
	}
	//PIN_PRINT("ImageLoad01");
	//RTN rtn01 = RTN_FindByName(img,"foo");
	/*if (RTN_Valid( rtn01 ))
	{
		RTN_Open(rtn01);
		RTN_InsertCall(rtn01, IPOINT_AFTER, AFUNPTR(AfterInitLock),
					   IARG_FUNCRET_EXITPOINT_VALUE ,IARG_END);
					   			   
		RTN_Close(rtn01);
	}*/
PIN_PRINT("ImageLoad1");	
	RTN rtn1 = RTN_FindByName(img, MutexLock);

	if (RTN_Valid( rtn1 ))
	{
		RTN_Open(rtn1);
        
		/*RTN_InsertCall(rtn1, IPOINT_BEFORE, AFUNPTR(BeforeGetLock),
					   IARG_THREAD_ID, 
					   IARG_FUNCARG_ENTRYPOINT_VALUE,0,
					   IARG_END);*/
		RTN_InsertCall(rtn1, IPOINT_AFTER, AFUNPTR(BeforeGetLock),
					   IARG_THREAD_ID,
					   IARG_FUNCARG_ENTRYPOINT_VALUE,0,
					   IARG_FUNCRET_EXITPOINT_VALUE ,IARG_END);			   

		RTN_Close(rtn1);
	}
PIN_PRINT("ImageLoad2");	
	RTN rtn2 = RTN_FindByName(img, MutexUnlock);

	if (RTN_Valid( rtn2 ))
	{
		RTN_Open(rtn2);
/*
		RTN_InsertCall(rtn2, IPOINT_BEFORE, AFUNPTR(BeforeGetUnlock),
					   IARG_THREAD_ID, 
					   IARG_FUNCARG_ENTRYPOINT_VALUE,0,
					   IARG_END);
*/
		RTN_InsertCall(rtn2, IPOINT_AFTER, AFUNPTR(BeforeGetUnlock),
					   IARG_THREAD_ID,
					   IARG_FUNCARG_ENTRYPOINT_VALUE,0,
					   IARG_FUNCRET_EXITPOINT_VALUE ,IARG_END);			   

		RTN_Close(rtn2);
	}
PIN_PRINT("ImageLoad3");
	RTN rtn3 = RTN_FindByName(img, MutexDestroy);
	if(RTN_Valid( rtn3 ))
	{
		RTN_Open(rtn3);
/*
		RTN_InsertCall(rtn3, IPOINT_BEFORE, AFUNPTR(BeforeDestroyLock),
					   IARG_FUNCARG_ENTRYPOINT_VALUE,0,
					   IARG_THREAD_ID, IARG_END);
*/
		RTN_InsertCall(rtn3, IPOINT_AFTER, AFUNPTR(BeforeDestroyLock),
					   IARG_FUNCARG_ENTRYPOINT_VALUE,0,
					   IARG_THREAD_ID,
					   IARG_FUNCRET_EXITPOINT_VALUE ,IARG_END);	
		RTN_Close(rtn3);
	}
PIN_PRINT("ImageLoad");
}

VOID Instruction(INS ins, VOID *v)
{
	// Instruments memory accesses using a predicated call, i.e.
	// the instrumentation is called iff the instruction will actually be executed.
	//
	// On the IA-32 and Intel(R) 64 architectures conditional moves and REP 
	// prefixed instructions appear as predicated instructions in Pin.
	UINT32 memOperands = INS_MemoryOperandCount(ins);


	// Iterate over each memory operand of the instruction.
	for (UINT32 memOp = 0; memOp < memOperands; memOp++)
	{
		if (INS_MemoryOperandIsRead(ins, memOp))
		{
			UINT32 MemReadSize = INS_MemoryReadSize(ins);
			if(!INS_IsStackRead	(ins)){
				INS_InsertPredicatedCall(
					ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
					IARG_INST_PTR,
					IARG_MEMORYOP_EA, memOp,
					IARG_UINT32, MemReadSize,
					IARG_THREAD_ID,
					IARG_END);
				}//else printf("stack read\n");
		}
		// Note that in some architectures a single memory operand can be 
		// both read and written (for instance incl (%eax) on IA-32)
		// In that case we instrument it once for read and once for write.
		if (INS_MemoryOperandIsWritten(ins, memOp))
		{
			UINT32 MemWriteSize = INS_MemoryWriteSize(ins);
			//printf("writesize %d\n",MemWriteSize);
			if(!INS_IsStackWrite(ins)){
				INS_InsertPredicatedCall(
					ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
					IARG_INST_PTR,
					IARG_MEMORYOP_EA, memOp,
					IARG_UINT32, MemWriteSize,
					IARG_THREAD_ID,
					IARG_END);
				}//else { printf("stack write\n");}
		}
	}
}

// This routine is executed once at the end.
VOID Fini(INT32 code, VOID *v)
{
	PIN_PRINT("end program");
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
	PIN_ERROR("This Pintool prints a trace of malloc calls in the guest application\n"+ KNOB_BASE::StringKnobSummary() + "\n");
	return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(INT32 argc, CHAR **argv)
{
PIN_PRINT("main");
   file = fopen("race.log","wb");
   test = fopen("test.log","wb");
   if(file == NULL) printf("打开文件失败\n");
   if(test == NULL) printf("打开文件失败\n");
	// Initialize the pin lock
	PIN_InitLock(&lock);

	// Initialize the LockLink	
	LL = inilink();

	// Initialize pin
	if (PIN_Init(argc, argv)) return Usage();
	PIN_InitSymbols();

	// Register ImageLoad to be called when each image is loaded.
	IMG_AddInstrumentFunction(ImageLoad, 0);

PIN_PRINT("main");
	//
	INS_AddInstrumentFunction(Instruction, 0);

PIN_PRINT("main");
	// Register Analysis routines to be called when a thread begins/ends
	PIN_AddThreadStartFunction(ThreadStart, 0);
PIN_PRINT("main");
	PIN_AddThreadFiniFunction(ThreadFini, 0);

PIN_PRINT("main");
	// Register Fini to be called when the application exits
	PIN_AddFiniFunction(Fini, 0);

PIN_PRINT("main");
	// Never returns
	PIN_StartProgram();
	fclose(file);
	fclose(test);
	return 0;
}
