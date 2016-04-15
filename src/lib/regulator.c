#include "regulator.h"

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

		pthread_mutex_lock(regul->mutex);

		pthread_mutex_unlock(regul->mutex);
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