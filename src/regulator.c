#define _XOPEN_SOURCE 500

#include "regulator.h"
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

#include <pthread.h>
#include <stdlib.h>

void* run_regul(void *arg)
{
	thread_args_t *thread_args = (thread_args_t*) arg;
	regul_t *regul = thread_args->regul;
	data_t *data = thread_args->data;

	init();

	/*Big rotor*/
	analogInOpen(0);
	analogOutOpen(0);

	/*Small rotor*/
	analogInOpen(1);
	analogOutOpen(1);

	clock_t begin, end;

	double u_pitch, u_yaw, y_pitch, y_yaw;
	double h = 2; //Sample time [s] of the regulator. Set better value.
	while(thread_args->run) {
		if (!regul->on) //Bad, doesn't take sleep() into account
			continue;
		/* TODO
		 * This shouldnt be here, if we start late the next will be
		 * late too
		 */
		begin = clock();

		/* TODO
		 * Switch for read_data() from the input here later.
		 */

		pthread_mutex_lock(regul->mutex);

		/* Control algorithm and write output */
		y_pitch = analogIn(0);
		y_yaw = analogIn(1);

		/* Second Kalman
		 * Read reference (Trajectory planning might deal with
		 * incremental reference changes). Calculate output (LQG/MPC)
		 */
		u_pitch = 0;
		u_yaw = 0;
		u_pitch = limit(u_pitch); //Make u somewhere
		u_yaw= limit(u_yaw);
		analogOut(0, u_pitch);
		analogOut(1, u_yaw);

		/* Push output, Kalman 1 and trajectory planning */

		pthread_mutex_unlock(regul->mutex);

		/* Write to buffer. Lock buffer mutex */
		pthread_mutex_lock(data->mutex);
		write_data(u_pitch, u_yaw, y_pitch, y_yaw, data);
		pthread_mutex_unlock(data->mutex);

		/* Handle sleep */
		end = clock();

		long int sleep_time = 1000000 * (
				h - (double)(end-begin) / CLOCKS_PER_SEC);
		if (sleep_time < 0) {
			sleep_time = 0;
		}
		int stuff = usleep(sleep_time);
		if (stuff < 0) {
			printf("usleep failed. Exiting.\n");
			return NULL;
		} else {
			printf("Regul sleeping for %ld micro s .\n",sleep_time);
		}
	}
	/*Close all ports and set output to zero*/
	analogOut(0,0);
	analogOut(1,0);
	analogInClose(0);
	analogOutClose(0);
	analogInClose(1);
	analogOutClose(1);
	return NULL;
}

regul_t *init_regul()
{
	regul_t *regul = (regul_t*) malloc(sizeof(regul_t));
	regul->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(regul->mutex, NULL);
	return regul;
}

void free_regul(regul_t *regul)
{
	pthread_mutex_destroy(regul->mutex);
	free(regul->mutex);
	free(regul);
}

data_t *init_data()
{
	data_t *data = (data_t*) malloc(sizeof(data_t));
	data->buffer = (double*) malloc(400 * sizeof(double));
	data->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(data->mutex, NULL);
	data->nbr_of_datapoints = 0;
	return data;
}

void free_data(data_t *data)
{
	free(data->buffer);
	pthread_mutex_destroy(data->mutex);
	free(data->mutex);
	free(data);
}

void add_data(data_t *data)
{
	;
}

double limit(double u)
{
	if (u > 10.0)
		u = 10.0;
	else if (u < -10.0)
		u = -10.0;

	return u;
}

void write_data(double u_pitch, double u_yaw, double y_pitch, double y_yaw,
		data_t* data)
{
	*((data->buffer) + data->nbr_of_datapoints * 4) = u_pitch;
	*((data->buffer) + data->nbr_of_datapoints * 4 + 1) = u_yaw;
	*((data->buffer) + data->nbr_of_datapoints * 4 + 2) = y_pitch;
	*((data->buffer) + data->nbr_of_datapoints * 4 + 3) = y_yaw;
	data->nbr_of_datapoints += 1;
}
