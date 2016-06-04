#ifdef _MSC_VER
//#define _CRT_SECURE_NO_WARNINGS
#define _CRT_RAND_S
#endif

#include <stdlib.h>
#include <stdint.h>
#include <Windows.h>

#define PREFIX1_LOCK 0x0fu
#define PREFIX1_REPNE 0xf2u
#define PREFIX1_REP 0xf3u
#define PREFIX2_CSOV 0x2eu
#define PREFIX2_SSOV 0x36u
#define PREFIX2_DSOV 0x3eu
#define PREFIX2_ESOV 0x26u
#define PREFIX2_FSOV 0x64u
#define PREFIX2_GSOV 0x65u
#define PREFIX2_BRANCH_TAKEN 0x2eu
#define PREFIX2_BRANCH_NOT_TAKEN 0x3eu
#define PREFIX3_OPSIZEOV 0x66
#define PREFIX4_ADDRSIZEOV 0x67

#define PREFIX0_VEX3 0xC4
#define PREFIX0_VEX2 0xC5
#define PREFIX0_XOP3 0x8f

#define OP_XOP_VPPERM 0xa30000

#define BLOCK_SIZE 0x10000

void* gen_xop_set()
{
	void* pages = VirtualAlloc(NULL, BLOCK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	uint64_t* nextfree = (uint64_t*)pages;

	register uint64_t random = 0;

	while ((uint8_t*)nextfree < (uint8_t*)pages + BLOCK_SIZE - 8)
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
	}

	*(uint8_t*)nextfree = 0xc3;
	while (!(*((uint8_t*)nextfree + 1)))
	{	
		nextfree = (uint64_t*)((uint8_t*)nextfree + 1);
		*(uint8_t*)nextfree = 0xcc;
	}
	return pages;
}

int main(void)
{
	gen_xop_set();
	return 0;
}