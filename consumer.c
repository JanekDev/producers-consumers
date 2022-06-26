#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "circular_q.h"

int main() {

	int psize = sysconf(_SC_PAGE_SIZE);
	int fd, product;
	char *buffer;
	queue *free_q, *taken_q;
	sem_t *Sp, *Sc;

	// memory-mapping the shared buffer
	fd = shm_open("/buffer", O_CREAT|O_RDWR, 0600);
	ftruncate(fd, psize*3);
	
	buffer = mmap(NULL, N*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	taken_q = mmap(NULL, sizeof(queue), PROT_READ|PROT_WRITE, MAP_SHARED, fd, psize);
	free_q = mmap(NULL, sizeof(queue), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 2*psize);


	// error handling
	if (buffer == MAP_FAILED || taken_q == MAP_FAILED || free_q == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	Sp = sem_open("/Sp", O_RDWR);
	Sc = sem_open("/Sc", O_RDWR);

	if (Sp==SEM_FAILED || Sc==SEM_FAILED) perror("sem_open");

	while(1) {
		sem_wait(Sc);
		product = buffer[taken_q->removal];
		printf("Consumer %d consumed %d from %d\n", getpid(), product, taken_q->removal);
		free_q->queue[free_q->addition] = taken_q->queue[taken_q->removal];
		taken_q->removal = (taken_q->removal + 1)%N;
		free_q->addition = (free_q->addition + 1)%N;
		//sleep(2); // for visual tests
		sem_post(Sp);
	}

	sem_close(Sp);
	sem_close(Sc);
	sem_unlink("/Sp");
	sem_unlink("/Sc");
	munmap(buffer, N*sizeof(int));
	munmap(free_q, sizeof(queue));
	munmap(taken_q, sizeof(queue));
	shm_unlink("/buffer");
	return 0;
}
