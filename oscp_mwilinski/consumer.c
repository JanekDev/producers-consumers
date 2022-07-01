#include "circular_q.h"

int main() {

	int psize = sysconf(_SC_PAGE_SIZE);
	int fd, product, sleep_time;
	char *buffer;
	queue *free_q, *taken_q;
	sem_t *Sp, *Sc, *Sqc;

	// random time between 1 and 5 seconds
	srand ( time(NULL)%getpid() );
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

	Sp = sem_open("/Sp", O_RDWR);
	Sc = sem_open("/Sc", O_RDWR);
	Sqc = sem_open("/Sqc", O_CREAT, 0600, 1);
	
	if (Sp==SEM_FAILED || Sc==SEM_FAILED) perror("sem_open");

	while(1) {
		sem_wait(Sc);
		sem_wait(Sqc);
		// FIXED the line below lacked just the reading of index from the queue not the removal number itself
		product = buffer[taken_q->queue[taken_q->removal]];
		// FIXED also the same problem as above but in printing the index
		printf("Consumer %d consumed %d from index %d\n", getpid(), product, taken_q->queue[taken_q->removal]);
		free_q->queue[free_q->addition] = taken_q->queue[taken_q->removal];
		taken_q->removal = (taken_q->removal + 1)%N;
		free_q->addition = (free_q->addition + 1)%N;
		usleep(sleep_time); // for visual tests
		sem_post(Sp);
		sem_post(Sqc);
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
