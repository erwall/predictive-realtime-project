#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct Threadargs {
	int* number;
	pthread_mutex_t *mx;
	int* ready;
} Threadargs;


void *printstuff(void *argin) {
	Threadargs *arg = (Threadargs*) argin;
	pthread_mutex_t mx = *arg->mx;
	int *ready = arg->ready;
	int*n = arg->number;
	printf("Initalizaing printstuff \n");
	while (*n<10) {
		usleep(150);
		pthread_mutex_lock(&mx);
		if(*ready) {
			printf("Thread printing %d \n", *n);
			*ready = 0;
		}
		pthread_mutex_unlock(&mx);
	}
}

void *increase_stuff(void *argin) {
	printf("initializaing increase stuff\n");
	Threadargs *arg = (Threadargs*) argin;
	pthread_mutex_t mx = *arg->mx;
	int *n = arg->number;
	int *ready = arg->ready;
	while (*n<10) {	
		pthread_mutex_lock(&mx); //critical section
		*ready = 1;
		*n = *n + 1;
		pthread_mutex_unlock(&mx);
		usleep(1000);
	}
}

int main(int argc, char *argv[]) {
	pthread_t print,increase;
	int rc1, rc2;
	int number;
	int r = 1;
	int* rp = &r;
	int* t = &number;
	*t = 0;
	pthread_mutex_t mx;
	pthread_mutex_init(&mx,NULL);
	Threadargs stuff;
	stuff.number = t;
	stuff.mx = &mx;
	stuff.ready = rp;
	printf("creating some threads \n");

	rc1 = pthread_create(&print,NULL,printstuff,(void *)&stuff);
	if(rc1)
		printf("Error thread 1");
	rc2 = pthread_create(&increase,NULL,increase_stuff,(void *)&stuff);
	if (rc2)
		printf("Error thread 2");

	pthread_join(print,NULL);
	pthread_join(increase,NULL);	
	pthread_mutex_destroy(&mx);
	exit(0);
}

