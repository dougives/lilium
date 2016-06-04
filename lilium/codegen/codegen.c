#ifdef _MSC_VER
//#define _CRT_SECURE_NO_WARNINGS
#define _CRT_RAND_S
#endif

#include <stdlib.h>
#include <stdint.h>
#include <Windows.h>

#define PREFIX1_LOCK 0x0f
#define PREFIX1_REPNE 0xf2
#define PREFIX1_REP 0xf3
#define PREFIX2_CSOV 0x2e
#define PREFIX2_SSOV 0x36
#define PREFIX2_DSOV 0x3e
#define PREFIX2_ESOV 0x26
#define PREFIX2_FSOV 0x64
#define PREFIX2_GSOV 0x65
#define PREFIX2_BRANCH_TAKEN 0x2e
#define PREFIX2_BRANCH_NOT_TAKEN 0x3e
#define PREFIX3_OPSIZEOV 0x66
#define PREFIX4_ADDRSIZEOV 0x67

#define PREFIX0_VEX3 0xc4
#define PREFIX0_VEX2 0xc5
#define PREFIX0_XOP3 0x8f

#define OP_XOP_VPPERM 0xa30000

#define BLOCK_SIZE 0x10000

#define MAX_GENSIZE 0x20

size_t gen_input(void* start)
{
	uint8_t* nextfree = (uint8_t*)start;
	uint32_t random = 0;
	while (rand_s(&random)) {}
	random &= 0x0f;
	*nextfree++ = 0x66;
	if (random & 0x08)
		*nextfree++ = 0x44;
	// prefix, mov
	*((uint16_t*)nextfree)++ = 0x6f0fu;
	*nextfree++ = 0x06 + ((random & 0x07) << 3);
	*((uint32_t*)nextfree)++ = 0x10c68348u; // add rsi, 0x10
	*((uint32_t*)nextfree)++ = 0x7cf03b48u; // cmp, rsi, rax   
	*((uint16_t*)nextfree)++ = 0xc301u;		// jl 0x01
											// ret (output is full)
	return (size_t)(nextfree - (uint8_t*)start);
}

size_t gen_header(void* start)
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

	uint64_t* nextfree = (uint64_t*)start;
	*nextfree++ = 0x8b48f18b48535756uL;
	*nextfree++ = 0x48c18b48c88b49fauL;
	*nextfree++ = 0xd70348c60348d18buL;
	return (size_t)((uint8_t*)nextfree - (uint8_t*)start);
}

size_t gen_xop_vpperm(void* start)
{
	uint64_t* nextfree = (uint64_t*)start;
	uint64_t random = 0;
	size_t written = 0;
	random ^= random >> 13;
	while (rand_s((uint32_t*)&random)) {}
	random <<= 32;
	while (rand_s((uint32_t*)&random)) {}
	// vpperm: xop ~rxb.08 W.src1.000 0xa3 /r ib
	// but backwards ...
	// and out junk, or in goods
	random &= 0xf03f00f8e000u;
	random |= 0x00c0a300088fu;
	*nextfree = random;
	nextfree = (uint64_t*)((uint8_t*)nextfree + 6);
	written += 6;
	return written;
}

void* gen_xop_set()
{
	uint32_t random = 0;
	void* pages = VirtualAlloc(NULL, BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	uint8_t* nextfree = (uint8_t*)pages;
	nextfree += gen_header((void*)nextfree);

	while (nextfree < (uint8_t*)pages + BLOCK_SIZE - MAX_GENSIZE - 1)
	{
		while (rand_s(&random)) {}
		random &= 0x0f;
		if (random == 0x0f)
		{
			nextfree += gen_input((void*)nextfree);
			continue;
		}

		nextfree += gen_xop_vpperm((void*)nextfree);
	}



	*nextfree = 0xc3;
	while (!*++nextfree
		&& nextfree < (uint8_t*)pages + BLOCK_SIZE)
	{	
		*nextfree = 0xcc;
	}
	return pages;
}

int main(void)
{
	void (*pages)();
	pages = gen_xop_set();
	pages();
	return 0;
}