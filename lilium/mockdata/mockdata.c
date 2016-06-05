#ifdef _MSC_VER
//#define _CRT_SECURE_NO_WARNINGS
#define _CRT_RAND_S
#endif

#include <stdio.h>
#include <stdint.h>
#include <immintrin.h>
#include <stdlib.h>
#include <Windows.h>

#define FILENAME "f:\\lilium\\mock.solution"
#define SIZE (0x40000000 >> 2)

int main(void)
{
	FILE* file;
	int error = fopen_s(&file, FILENAME, "w+");
	uint32_t random;
	size_t i;
	for (i = 0; i < SIZE; i++)
	{
		while (rand_s(&random))
		{ }
		fwrite(&random, sizeof(random), 1, file);
	}
	_fcloseall();
	return 0;
}