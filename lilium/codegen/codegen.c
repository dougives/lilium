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

	uint64_t* ulptr = (uint64_t*)start;
	*ulptr++ = 0x8b48f18b48535756uL;
	*ulptr++ = 0x48c18b48c88b49fauL;
	*ulptr++ = 0xd70348c60348d18buL;
	return 24;
}

size_t gen_xop_vpperm(void* start, size_t size)
{
	uint64_t* nextfree = (uint64_t*)start;
	uint64_t random = 0;
	size_t written = 0;
	while ((uint8_t*)nextfree < (uint8_t*)start + size - 8)
	{
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
	}
	return written;
}

void* gen_xop_set()
{
	void* pages = VirtualAlloc(NULL, BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	uint8_t* nextfree = (uint8_t*)pages;
	nextfree += gen_header((void*)nextfree);
	nextfree += gen_xop_vpperm((void*)nextfree, BLOCK_SIZE - (nextfree - pages));
	*nextfree = 0xcc;
	while (!*++nextfree)
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