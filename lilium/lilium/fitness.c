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

void fit_func_variance(Creature* cre, size_t outpcount)
{
	if (!fit_func_check(cre, outpcount))
	{
		fit_report(0);
		return;
	}

	outpcount >>= 1;

	__m128i* outp = (__m128i*)cre->output;
	__m128i temp = _mm_setzero_si128();
	__m128i* solu = (__m128i*)solution;
	__m128i zero = _mm_setzero_si128();
	__m128i ones = { 1, 1 };
	__m128i mulhi = _mm_setzero_si128();
	__m128i mullo = _mm_setzero_si128();
	__m128i cmp = _mm_setzero_si128();
	//__m256i product = _mm256_setzero_si256();
	//__m256i workingsum = _mm256_setzero_si256();
	//__m256i totalsum = _mm256_setzero_si256();
	__m128i wsumhi = _mm_setzero_si128();
	__m128i wsumlo = _mm_setzero_si128();
	__m128i tsumhi = _mm_setzero_si128();
	__m128i tsumlo = _mm_setzero_si128();

	for (size_t i = 0; i < outpcount; i++)
	{
		temp = _mm_sub_epi64(outp[i], solu[i]);
		// if sign bit is set, get absolute value
		cmp = _mm_cmpgt_epi64(zero, temp);
		temp = _mm_xor_si128(temp, cmp);
		temp = _mm_add_epi64(temp, ones);
		// should be abs now, square...
		mullo = _mm_mul_epu32(temp, temp);
		temp = _mm_srli_epi64(temp, 32);
		mulhi = _mm_mul_epu32(temp, temp);
		// product.m256i_i64[0] = mullo.m128i_i64[0];
		// product.m256i_i64[1] = mullo.m128i_i64[1];
		// product.m256i_i64[2] = mulhi.m128i_i64[0];
		// product.m256i_i64[3] = mulhi.m128i_i64[1];
		// summing > 0x1000 could overflow
		// so we need to break the shifts into
		// small steps to do a whole gig
		// so every 0x1000, >> by 12
		
		wsumlo = _mm_add_

		if (i & 0x0fff == 0x0fff)
		{
			//_mm_
		}
	}
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