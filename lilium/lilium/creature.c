#ifdef _MSC_VER
#define _CRT_RAND_S
#endif

#include <stdint.h>
#include <Windows.h>
#include <nmmintrin.h>
#include <stdbool.h>
#include "codegen.h"
#include "atomic.h"
#include "crehelper.h"

#define CRE_STATE_空 0x00000000u
#define CRE_STATE_生 0x00000001u
#define CRE_STATE_活 0x00000002u
#define CRE_STATE_死 0x00000004u

#define MIN_INPUT_SIZE 0x100

//#define DEFAULT_POOL_SIZE 0x02

typedef struct
{
	volatile uint64_t lock;
	volatile uint32_t state;
	void* block;
	void* output;
} Creature;

typedef struct
{
	size_t size;
	Creature** slots;
} CreaturePool;

static CreaturePool pool;
static size_t sizemask;
static bool initialized = false;
static bool running = false;
static void* inpdata = NULL;
static size_t inpsize = 0;

static void cre_exec_thread(Creature* cre)
{
	crehelper_fill_xmmregs(inpdata);
	// ((void(*)(void*, void*, size_t))cre->block)(inpdata, cre->output, )
}

static void cre_pool_thread()
{
	running = true;
	size_t index = 0;
	Creature* cre = NULL;
	while (running)
	{
		index = ++index & sizemask;
		if ((cre = pool.slots[index])->state != CRE_STATE_生)
			continue;
		
		if (!atomic_lock(&cre->lock))
			continue;

		cre->state = CRE_STATE_活;
		
		// need to load start of file into xmm regs
		// crehelper_fill_xmmregs(inpdata);

		// DWORD threadid = 0;
		// CreateThread(NULL, 0, cre->block, NULL, 0, &threadid);
		////////////////////////////////////////////////////
	}

	return;
}

DWORD cre_stop_pool()
{
	return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD cre_start_pool()
{
	return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD cre_birth(void* (*genset)(BoundBuffer*))
{
	if (!initialized)
		return ERROR_INVALID_FUNCTION;

	uint32_t random = 0;
	DWORD lasterror = ERROR_SUCCESS;
	BoundBuffer* bbuf;
	Creature* cre = NULL;
	
	lasterror = rand_s(&random);
	if (lasterror != ERROR_SUCCESS)
		return lasterror;

	lasterror = gen_create_boundbuffer(bbuf);
	if (lasterror != ERROR_SUCCESS)
		return lasterror;

	void* block = genset(bbuf);
	if (block == NULL)
		return GetLastError();

	do 
	{ 
		random = ++random & sizemask;
	} while (!
		(cre->state == CRE_STATE_空 
		&& atomic_lock(&pool.slots[random]->lock)));
	
	cre = pool.slots[random];
	cre->block = block;
	cre->lock = ATOMIC_UNLOCKED;
	cre->state = CRE_STATE_生;

	return ERROR_SUCCESS;
}

DWORD cre_init(size_t poolsize, const HANDLE inputdata, const size_t inputsize)
{
	if (inputdata == NULL || inputsize < MIN_INPUT_SIZE)
		return ERROR_BAD_ARGUMENTS;

	if (poolsize < 1 
		|| _mm_popcnt_u32((uint32_t)poolsize) != 1) // is not pow of 2?
		return ERROR_BAD_LENGTH;

	inpdata = inputdata;
	inpsize = inputsize;
	sizemask = poolsize - 1;
	pool.size = poolsize;
	pool.slots = calloc(poolsize, sizeof(CreaturePool));
	if (pool.slots == NULL)
		return ERROR_INVALID_HANDLE;

	initialized = true;
	return ERROR_SUCCESS;
}



