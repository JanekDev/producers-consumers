#ifndef circular_q_h
#define circular_q_h

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#define N 5 

typedef struct queue{
	int addition;
	int removal;
	int queue[N];
	
} queue;

#endif