#include "circular_q.h"

int main() {
	int psize = sysconf(_SC_PAGE_SIZE);
	int fd, product, sleep_time, pop;
	char *buffer;
	queue *free_q, *taken_q;
	sem_t *Sp, *Sc, *Sqf, *Sqt;

	//random generator had to be seeded with something more than current time
	srand ( time(NULL) % getpid() );
	sleep_time = (rand() % 5000)*1000;

	printf("sleep_time: %d ms\n", sleep_time/1000);

	// memory-mapping the shared buffer
	fd = shm_open("/buffer", O_RDWR, 0600);
	ftruncate(fd, psize*3);

	buffer = mmap(NULL, N*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	taken_q = mmap(NULL, sizeof(queue), PROT_READ|PROT_WRITE, MAP_SHARED, fd, psize);
	free_q = mmap(NULL, sizeof(queue), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 2*psize);
	
	// error handling
	if (buffer == MAP_FAILED || taken_q == MAP_FAILED || free_q == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	// initialize binary accessible semaphores
	Sp = sem_open("/Sp", O_CREAT, 0600, N);
	Sc = sem_open("/Sc", O_CREAT, 0600, 0);
	Sqf = sem_open("/Sqf", O_CREAT, 0600, 1);
	Sqt = sem_open("/Sqt", O_CREAT, 0600, 1);
	
 
	if (Sp==SEM_FAILED || Sc==SEM_FAILED || Sqt==SEM_FAILED || Sqf==SEM_FAILED) perror("sem_open");

	product = 0;
	
	while(1) {
		// wait for a free buffer slot
		sem_wait(Sp);
		product = (product + 1) % 128;
		// FIX added the queue access semaphores for the producer
		sem_wait(Sqf);
		pop = free_q->queue[free_q->removal];
		free_q->removal = (free_q->removal + 1) % N;
		sem_post(Sqf);
		sem_wait(Sqt);
		taken_q->queue[taken_q->addition] = pop;
		taken_q->addition = (taken_q->addition + 1) % N;
		sem_post(Sqt);
		// this action doesn't need to be protected by the queue access semaphore
		// because the mutual exclusion of indexes is ensured by the semaphore Sqp
		// with the bound on the number of producers Sp
		// FIX corrected the producing index bug
		buffer[pop] = product; 
		printf("Producer %d produced %d and put it into index %d\n", getpid(), product, pop);
		usleep(sleep_time); // for visual tests
		sem_post(Sc);
	}
	sem_close(Sp);
	sem_close(Sc);
	sem_close(Sqt);
	sem_close(Sqf);
	sem_unlink("/Sp");
	sem_unlink("/Sc");
	sem_unlink("/Sqf");
	sem_unlink("/Sqt");
	munmap(buffer, N*sizeof(int));
	munmap(taken_q, sizeof(queue));
	munmap(free_q, sizeof(queue));
	shm_unlink("/buffer");
	return 0;
}
