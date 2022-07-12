#include "circular_q.h"


int main() {
	
	int psize = sysconf(_SC_PAGE_SIZE);
	int fd, i;
	char *buffer;
	queue *free_q, *taken_q;
	sem_t *Sp, *Sc, *Sqf, *Sqt;

	
	// memory-mapping the shared buffer
	fd = shm_open("/buffer", O_CREAT | O_RDWR, 0600);
	ftruncate(fd, psize*3);
	buffer = mmap(NULL, N*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	taken_q = mmap(NULL, sizeof(queue), PROT_READ|PROT_WRITE, MAP_SHARED, fd, psize);
    free_q = mmap(NULL, sizeof(queue), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 2*psize);

	// error handling
	if (buffer == MAP_FAILED || taken_q == MAP_FAILED || free_q == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

	// create semaphores
	Sp = sem_open("/Sp", O_CREAT, 0600, N);
	Sc = sem_open("/Sc", O_CREAT, 0600, 0);
	Sqf = sem_open("/Sqf", O_CREAT, 0600, 1);
	Sqt = sem_open("/Sqt", O_CREAT, 0600, 1);
	
 
	if (Sp==SEM_FAILED || Sc==SEM_FAILED || Sqt==SEM_FAILED || Sqf==SEM_FAILED) perror("sem_open");


	// initialize shared variables
	i=0;
	for (i=0; i<N; i++) {
		free_q->queue[i] = i;
        taken_q->queue[i] = -1;
	}
	free_q->addition = 0;
    free_q->removal = 0;
    taken_q->addition = 0;
    taken_q->removal = 0;
}
