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
#include "creature.h"


//#define DEFAULT_POOL_SIZE 0x02

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
static size_t outsize = 0;

static void cre_exec_thread(Creature* cre)
{
	void* outptr = NULL;
	cre->output = malloc(outsize);
	crehelper_fill_xmmregs(inpdata);
	outptr = ((void*(*)(void*, void*, size_t, size_t))cre->block)(
		(uint8_t*)inpdata + 0x100,
		cre->output,
		inpsize,
		outsize);

	// should check outptr for null or oddities ...
	// fitness needs to free output when done
	cre->fitnessfunc(cre, (size_t)((uint64_t*)outptr - (uint64_t*)cre->output));
	return;
}

static void cre_pool_thread()
{
	running = true;
	size_t index = 0;
	Creature* cre = NULL;
	DWORD threadid = 0;

	while (running)
	{
		index = ++index & sizemask;
		if ((cre = pool.slots[index])->state != CRE_STATE_生)
			continue;
		
		if (!atomic_lock(&cre->lock))
			continue;

		cre->state = CRE_STATE_活;

		cre->thread = CreateThread(NULL, 0, cre->block, NULL, 0, &threadid);
		if (cre->thread == NULL)
		{
			cre->lock = ATOMIC_UNLOCKED;
			cre->state = CRE_STATE_死;
			//continue; // should report this somehow ...
		}
	}

	return;
}

DWORD cre_stop_pool()
{
	if (!initialized)
		return ERROR_INVALID_FUNCTION;
	return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD cre_start_pool()
{
	if (!initialized)
		return ERROR_INVALID_FUNCTION;
	return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD cre_birth(void* (*genset)(BoundBuffer*), void (*fitnessfunc)(Creature*, size_t))
{
	if (!initialized)
		return ERROR_INVALID_FUNCTION;

	if (genset == NULL || fitnessfunc == NULL)
		return ERROR_BAD_ARGUMENTS;

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
	cre->thread = NULL;
	cre->fitnessfunc = fitnessfunc;
	cre->lock = ATOMIC_UNLOCKED;
	cre->state = CRE_STATE_生;

	return ERROR_SUCCESS;
}

DWORD cre_init(size_t poolsize, const HANDLE inputdata, const size_t inputsize, const size_t outputsize)
{
	if (inputdata == NULL 
		|| inputsize < MIN_INPUT_SIZE 
		|| outputsize < MIN_OUTPUT_SIZE)
		return ERROR_BAD_ARGUMENTS;

	if (poolsize < 1 
		|| _mm_popcnt_u32((uint32_t)poolsize) != 1) // is not pow of 2?
		return ERROR_BAD_LENGTH;

	inpdata = inputdata;
	inpsize = inputsize;
	outsize = outputsize;
	sizemask = poolsize - 1;
	pool.size = poolsize;
	pool.slots = calloc(poolsize, sizeof(CreaturePool));
	if (pool.slots == NULL)
		return ERROR_INVALID_HANDLE;

	initialized = true;
	return ERROR_SUCCESS;
}



