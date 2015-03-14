/**
 * Multi-threaded prime number test
 *
 * Copyright (C) 2013  Your Name <your_netid@illinois.edu> 
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct _ptest_t {
} ptest_t;

#define IS_PRIME 0

/**
 * Tests if a value is prime by dividing the canidate prime by
 * every number in the inclusive range: [start, end].
 * 
 * @param ptr
 *    A pointer to the ptest_t data structure.
 *
 * @return
 *    A pointer to an integer type storing
 *    either the number that the prime canidate is divisible
 *    by or 0 if the prime canidate is not divisible by
 *    any number in the range [start, end].
 */
void *prime_tester(void *ptr)
{
  return NULL;
}

int main(int argc, char **argv)
{
	return 0;
}

