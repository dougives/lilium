#include <stdint.h>
#include <Windows.h>


#define CRE_STATE_空 0x00000000u
#define CRE_STATE_生 0x00000001u
#define CRE_STATE_活 0x00000002u
#define CRE_STATE_死 0x00000004u

#define MIN_INPUT_SIZE 0x1000
#define MIN_OUTPUT_SIZE 0x1000

typedef struct
{
	volatile uint64_t lock;
	volatile uint32_t state;
	HANDLE thread;
	void(*fitnessfunc)(struct Creature*, size_t);
	void* block;
	void* output;
} Creature;