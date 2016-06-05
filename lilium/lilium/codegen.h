#pragma once

#define BLOCK_SIZE 0x10000
#define BOUND_BUFFER_SIZE 0x10000

typedef struct
{
	void* bounds[BOUND_BUFFER_SIZE];
	size_t free;
	size_t count;
} BoundBuffer;

void* gen_xop_set(BoundBuffer* boundbuffer);
DWORD gen_create_boundbuffer(BoundBuffer* bbuf);
DWORD gen_destroy_boundbuffer(BoundBuffer* bbuf);
