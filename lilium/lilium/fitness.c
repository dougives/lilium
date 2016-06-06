#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include <immintrin.h>
#include <nmmintrin.h>
#include <emmintrin.h>
#include "creature.h"

static uint64_t* solution = NULL;
static size_t count = 0;
static bool initialized = false;

bool fit_func_check(Creature* cre, size_t outpcount)
{
	return initialized
		&& cre != NULL
		&& cre->output != NULL
		&& cre->thread != NULL
		&& cre->state == CRE_STATE_活
		&& outpcount == count;
}

void fit_report(uint64_t fitness)
{
	return;
}

uint64_t fit_func_silly_variance(Creature* cre, size_t outpcount)
{
	if (!fit_func_check(cre, outpcount))
	{
		fit_report(0);
		return;
	}

	outpcount >>= 1;

	int64_t result = 0;
	__m128i* outp = (__m128i*)cre->output;
	__m128i* solu = (__m128i*)solution;
	__m128i zero = _mm_setzero_si128();
	__m128i ones = { 1, 1 };
	__m128i cmp = _mm_setzero_si128();
	__m128i temp = _mm_setzero_si128();
	__m128i sum = _mm_setzero_si128();

	for (size_t i = 0; i < outpcount; i++)
	{
		temp = _mm_sub_epi64(outp[i], solu[i]);
		// if sign bit is set, get absolute value
		cmp = _mm_cmpgt_epi64(zero, temp);
		temp = _mm_xor_si128(temp, cmp);
		temp = _mm_add_epi64(temp, ones);
		// should be abs now, saturate into 32 bits
		temp = _mm_add_epi64(temp, _mm_srli_epi64(temp, 32));
		// to the pow of 2
		temp = _mm_mul_epi32(temp, temp);
		// saturate again ...
		temp = _mm_add_epi64(temp, _mm_srli_epi64(temp, 32));
		sum = _mm_add_epi64(sum, temp);
	}

	// average
	result = sum.m128i_i64[0] + sum.m128i_i64[1];
	result >>= _tzcnt_u64(outpcount);
	return result;
}

DWORD fit_init(uint64_t* sol, size_t cnt)
{
	if (sol == NULL || cnt < 1)
		return ERROR_BAD_ARGUMENTS;

	solution = sol;
	count = cnt;

	initialized = true;
	return ERROR_SUCCESS;
}