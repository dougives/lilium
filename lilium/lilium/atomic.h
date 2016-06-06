#pragma once
#include <stdbool.h>
#include <stdint.h>

#define ATOMIC_UNLOCKED 0

extern bool atomic_lock(volatile uint64_t* lock);
extern void atomic_unlock(volatile uint64_t* lock);
extern void atomic_increment(volatile uint64_t* addr);
