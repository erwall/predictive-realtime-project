#define _XOPEN_SOURCE 500

#include "gui.h"
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include "regulator.h"
#include <pthread.h>
#include <stdlib.h>

void* run_gui(void *arg)
{

	/* INIT */
	data_t *data_buffer = (data_t *) arg;
	double u1,u2,y1,y2;
	printf("test access %d \n", data_buffer->nbr_of_datapoints);
	while(1) {

		//Read data
		pthread_mutex_lock(data_buffer->mutex);
		if (data_buffer->nbr_of_datapoints >0) {
			for (int i = 0;i <data_buffer->nbr_of_datapoints;++i) {
				u1=*((data_buffer->buffer)+i*4);
				u2=*((data_buffer->buffer)+i*4+1);
				y1=*((data_buffer->buffer)+i*4+2);
				y2=*((data_buffer->buffer)+i*4+3); 
				//Do stuff with the data
				//TODO maybe put into vectors so we dont lock mutex for so long
				printf("u1 %f u2 %f y1 %f y2 %f \n",u1,u2,y1,y2);
			}
		} else {
			printf("data buffer empty at this time \n");
			
		}
		(data_buffer->nbr_of_datapoints) = 0;
		pthread_mutex_unlock(data_buffer->mutex);

	
		usleep(2000000); //Not really critical that we account for processing time

	}
	return NULL;

}


