#ifndef REGULATOR_H_
#define REGULATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <pthread.h>

typedef struct regul_t {
	pthread_mutex_t *mutex;
	double pitch_ref;
	double yaw_ref;
	char on;
} regul_t;

typedef struct data_t {
	/* Idea for buffer:
	Double vector of predetermined size. Put u1, u2, y1, y2 , then rinse and
	repeat.	Keep track of number of data points. If there are zero data
	points then it's empty and we cant plot. No plan to handle overflow,
	just make it big enough, wont take much memory anyway.
	*/
	pthread_mutex_t *mutex;
	int nbr_of_datapoints;
	double* buffer;
} data_t;

typedef struct thread_args_t {
	char run;
	regul_t *regul;
	data_t *data;
} thread_args_t;

/* Makes sure u is within it bounds */
double limit(double u);

/* Write data to buffer in data struct */
void write_data(double u_pitch, double u_yaw, double y_picth, double y_yaw,
		data_t *data_buffer);

/* Method run by the thread */
void* run_regul(void *regul);

/* Initialize regul_t struct. Allocates memory for struct and for members */
regul_t* init_regul();

/* Free regul_t members and struct */
void free_regul(regul_t *regul);

/* Initialize data_t struct. Allocates memory for struct and members */
data_t* init_data();

/* Free data_t members and struct */
void free_data(data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* REGULATOR_H_ */
