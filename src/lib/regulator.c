#include "regulator.h"
#include <sys/time.h>

void* run_regul(void *input_structs_temp)
{
	/*Initialization*/
	void **struct_pointer = (void **) input_structs_temp; //TODO charlie take a look at this
	regul_t *regul = (regul_t *) *struct_pointer ;
	data_t *data_buffer = (data_t *) *(struct_pointer+1);

	while(0) {
		//TODO if on
		regul->pitch_ref = 0;
		regul->yaw_ref = 0;		

		/*
		Switch for read_data() from the input here later.
		*/
		double h = 0,05; //Sample time [s] of the regulator. 
		timespec start, finish;
		double elapsed, t;
		pthread_mutex_lock(regul->mutex);
		clock_gettime(CLOCK_MONOTONIC, &start);

		/*
		control algorithm
		*/
		pthread_mutex_unlock(regul->mutex);
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = finish.tv_nsec - start.tv_nsec;
		t = h - (double)elapsed;
		if (nanosleep(t, NULL) < 0) {
			printf("Nanosleep failed.\n";
		} else {
			printf("Regul sleeping for %f.",t);
		}
	}
	return NULL;
}




regul_t* init_regul()
{
	regul_t *regul = (regul_t*) malloc(sizeof(regul_t));
	regul->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(regul->mutex,NULL);
	return regul;
}

void destroy_regul(regul_t *regul)
{
	pthread_mutex_destroy(regul->mutex);
	free(regul->mutex);
	free(regul);
}

data_t* init_data()
{
	data_t *data = (data_t *) malloc(sizeof(data_t));
	data->buffer = (double *) malloc(sizeof(double)*4*100);
	data->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(data->mutex,NULL);
	data->nbr_of_datapoints = 0;
	return data;
}

void destroy_data(data_t* data)
{
	free(data->buffer);
	pthread_mutex_destroy(data->mutex);
	free(data->mutex);
	free(data);
}