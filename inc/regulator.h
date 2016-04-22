#ifndef REGULATOR_H_
#define REGULATOR_H_

#include <stdlib.h>
#include <pthread.h>

typedef struct regul_t {
	double pitch_ref;
	double yaw_ref;
	pthread_mutex_t *mutex;
} regul_t;

typedef struct data_t {
	/*Idea for buffer:
	Double vector of predetermined size. Put u1, u2, y1, y2 , then rinse and repeat.
	Keep track of number of data points. If there are zero data point then its empty and we cant plot
	No plan to handle overflow, just make it big enuff, wont take much memory anyway.
	*/
	double* buffer;
	pthread_mutex_t *mutex;
	int nbr_of_datapoints;


} data_t;


/**
Makes sure u is within it bounds
*/
double limit(double u);

void write_data(double u_pitch,double u_yawp,double y_picth,double y_yaw,data_t *data_buffer);

/**
Method run by the thread
*/

void* run_regul(void *regul);


regul_t* init_regul();

void destroy_regul(regul_t *);

data_t* init_data();

void destroy_data(data_t *);



#endif /* REGULATOR_H_ */
