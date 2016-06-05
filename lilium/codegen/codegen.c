#ifdef _MSC_VER
//#define _CRT_SECURE_NO_WARNINGS
#define _CRT_RAND_S
#endif

#include <stdlib.h>
#include <stdint.h>
#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>

#define OP_JCCREL16 0x820f
#define OP_XOP	0x8f
#define BLOCK_SIZE 0x10000
#define MAX_GEN_SIZE 0x20
#define BOUND_BUFFER_SIZE 0x10000
#define BOUND_BUFFER_MASK (BOUND_BUFFER_SIZE - 1)

typedef struct
{
	void* bounds[BOUND_BUFFER_SIZE];
	size_t free;
	size_t count;
} BoundBuffer;

BoundBuffer boundbuffer;

void gen_add_boundry(void* boundry)
{
	boundbuffer.bounds[
		++boundbuffer.free 
		& BOUND_BUFFER_MASK] = boundry;
	boundbuffer.count = ++boundbuffer.count & BOUND_BUFFER_MASK;
	return;
}

size_t gen_jcc(void* ptr)
{
	uint32_t* nextfree = (uint8_t*)ptr;
	uint32_t random = 0;
	rand_s(&random);

	*((uint16_t*)nextfree)++ = OP_JCCREL16 | (random & 0x05);
	random >>= 8;

	*nextfree++ =
		(uint8_t*)boundbuffer.bounds[
			(boundbuffer.free - (random % boundbuffer.count - 1) - 1)
			& BOUND_BUFFER_MASK]
		- (uint8_t*)ptr;

	return (size_t)(nextfree - (uint8_t*)ptr);
}

size_t gen_sse41_ptest(void* ptr)
{
	uint8_t* nextfree = (uint8_t*)ptr;
	uint32_t random = 0;
	rand_s(&random);
	random &= 0xff;
	*nextfree++ = 0x66;	
	if (random & 0x05)
	{
		*nextfree++ = 0x40 | (random & 0x05);
	}

	*((uint32_t*)nextfree)++ = 0xc017380fu | (random & 0x3f000000u);
	return (size_t)(nextfree - (uint8_t*)ptr);
}


size_t gen_output(void* ptr)
{
	uint8_t* nextfree = (uint8_t*)ptr;
	uint32_t random = 0;
	rand_s(&random);
	random &= 0x0f;

	*((uint32_t*)nextfree)++ = 0x7cfa3b48u; // cmp, rsi, rax   
	*((uint16_t*)nextfree)++ = 0xc301u;		// jl 0x01
	*nextfree++ = 0x66;						// ret
	if (random & 0x08)
	{
		*nextfree++ = 0x44;
	}
	// prefix, mov
	*((uint16_t*)nextfree)++ = 0x7f0fu;
	*nextfree++ = 0x07 + ((random & 0x07) << 3);
	*((uint32_t*)nextfree)++ = 0x10c78348u; // add rdi, 0x10
	return (size_t)(nextfree - (uint8_t*)ptr);
}

size_t gen_input(void* ptr)
{
	uint8_t* nextfree = (uint8_t*)ptr;
	uint32_t random = 0;
	rand_s(&random);
	random &= 0x0f;

	*((uint32_t*)nextfree)++ = 0x7df03b48u; // cmp, rsi, rax   
	*nextfree++ = 0x08u;					// jge 0x08
	*nextfree++ = 0x66;
	if (random & 0x08)
		*nextfree++ = 0x44;
	// prefix, mov
	*((uint16_t*)nextfree)++ = 0x6f0fu;
	*nextfree++ = 0x06 + ((random & 0x07) << 3);
	*((uint32_t*)nextfree)++ = 0x10c68348u; // add rsi, 0x10
	return (size_t)(nextfree - (uint8_t*)ptr);
}

size_t gen_header(void* ptr)
{
	// rcx -> rsi -> start of input data
	// rdx -> rdi -> start of output data
	// r8  -> rcx -> size of data in bytes
	// rax -> input end
	// rdx -> output end
	// for loading/storing

	// push rsi
	// push rdi
	// push rbx
	// mov rsi, rcx
	// mov rdi, rdx
	// mov rcx, r8
	// mov rax, rcx
	// mov rdx, rcx
	// add rax, rsi
	// add rdx, rdi

	uint64_t* nextfree = (uint64_t*)ptr;
	*nextfree++ = 0x8b48f18b48535756uL;
	*nextfree++ = 0x48c18b48c88b49fauL;
	*nextfree++ = 0xd70348c60348d18buL;
	return (size_t)((uint8_t*)nextfree - (uint8_t*)ptr);
}

size_t gen_xop_vpcmov(void* ptr)
{
	// vpcmov xop ~rxb.08 W.vvvv.000 0xa2 /r ib[7:4]
	uint64_t* nextfree = (uint64_t*)ptr;
	uint64_t random = 0;
	size_t written = 0;
	rand_s((uint32_t*)&random);
	random <<= 32;
	rand_s((uint32_t*)&random);

	random &= 0x783f00f8e000u;
	random |= 0x00c0a200088fu;
	*nextfree = random;
	return 6;
}

size_t gen_xop_vpcomb(void* ptr)
{
	// vpcom xop ~rxb.08 0.vvvv.000 0xcc /r ib
	uint64_t* nextfree = (uint64_t*)ptr;
	uint64_t random = 0;
	size_t written = 0;
	rand_s((uint32_t*)&random);
	random <<= 32;
	rand_s((uint32_t*)&random);

	random &= 0x073f2378e000u;
	random |= 0x00c0cc00088fu;
	*nextfree = random;
	return 6;
}

size_t gen_xop_vprot(void* ptr)
{
	// vprot xop ~rxb.09 W.vvvv.000 [0x90-0x93] /r
	uint64_t* nextfree = (uint64_t*)ptr;
	uint64_t random = 0;
	size_t written = 0;
	rand_s((uint32_t*)&random);
	random <<= 32;
	rand_s((uint32_t*)&random);

	random &= 0x3f03f8e000u;
	random |= 0xc09000098fu;
	*nextfree = random;
	return 5;
}

size_t gen_xop_vpperm(void* ptr)
{
	uint64_t* nextfree = (uint64_t*)ptr;
	uint64_t random = 0;
	rand_s((uint32_t*)&random);
	random <<= 32;
	rand_s((uint32_t*)&random);
	// vpperm: xop ~rxb.08 W.src1.000 0xa3 /r ib
	// but backwards ...
	// and out junk, or in goods
	random &= 0xf03f00f8e000u;
	random |= 0x00c0a300088fu;
	*nextfree = random;
	return 6;
}


void* gen_xop_set()
{
	uint32_t random = 0;
	void* pages = VirtualAlloc(
		NULL, 
		BLOCK_SIZE, 
		MEM_COMMIT | MEM_RESERVE, 
		PAGE_EXECUTE_READWRITE);
	uint8_t* nextfree = (uint8_t*)pages;
	nextfree += gen_header((void*)nextfree);

	while (nextfree < (uint8_t*)pages + BLOCK_SIZE - MAX_GEN_SIZE - 1)
	{
		rand_s(&random);
		random &= 0x0f;
		if (random == 0x0f)
		{
			nextfree += gen_input((void*)nextfree);
			continue;
		}

		if (random == 0x00)
		{
			nextfree += gen_output((void*)nextfree);
			continue;
		}

		nextfree += gen_xop_vpperm((void*)nextfree);
	}

	*nextfree = 0xc3;
	while (!*++nextfree
		&& nextfree < (uint8_t*)pages + BLOCK_SIZE - 1)
	{	
		*nextfree = 0xcc;
	}
	return pages;
}

static int running = 1;

void thread()
{
	void* blocks[0x10000];
	for (int i = 0; i < 0x10000; i++)
	{
		blocks[i] = gen_xop_set();
	}
	printf(".");

	running--;
}


int main(void)
{
	memset(&boundbuffer, 0, sizeof(BoundBuffer));

	LPDWORD threadid = NULL;
	CreateThread(NULL, 0, thread, NULL, 0, threadid);
	//CreateThread(NULL, 0, thread, NULL, 0, threadid);
	//CreateThread(NULL, 0, thread, NULL, 0, threadid);
	//CreateThread(NULL, 0, thread, NULL, 0, threadid);
	//CreateThread(NULL, 0, thread, NULL, 0, threadid);
	//CreateThread(NULL, 0, thread, NULL, 0, threadid);
	//CreateThread(NULL, 0, thread, NULL, 0, threadid);
	//CreateThread(NULL, 0, thread, NULL, 0, threadid);
	while (running)
		Sleep(500);

	return 0;
}