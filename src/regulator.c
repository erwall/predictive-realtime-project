#include "regulator.h"
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define PREDICTOR_TEN (1)
#define FILTER_TEN (2)
#define PREDICTOR_16 (3)
#define FILTER_16 (4)

#define REGULATOR_LQ (0)
#define REGULATOR_MPC (1)

#define NS (1000000000l) /* Nanoseconds in a second */
#define h (50000000l) /* In nanoseconds, must be < 1 second */

#ifdef NO_HARDWARE
	/* Dummy implementations for compiling without hardware and regulator
	 * dependencies */
	void init() {};
	void analogInOpen(int channel) {};
	void analogOutOpen(int channel) {};
	double analogIn(int channel) {return 0.0;};
	void analogOut(int channel, double value) {};
	void analogInClose(int channel) {};
	void analogOutClose(int channel) {};
	void Kalman(int filter, double y_pitch, double y_yaw,
			double u_pitch, double u_yaw, double states[16]) {};
#else
#include "io.c"
#endif

void* run_regul(void *arg)
{
	thread_args_t *thread_args = (thread_args_t*) arg;
	regul_t *regul = thread_args->regul;
	data_t *data = thread_args->data;

	init();
	double states[16];
	char regulator = 0;

	/*Big rotor*/
	analogInOpen(0);
	analogOutOpen(0);

	/*Small rotor*/
	analogInOpen(1);
	analogOutOpen(1);

	struct timespec ts = { .tv_sec = time(NULL), .tv_nsec = 0 };

	double u_pitch, u_yaw, y_pitch, y_yaw;

	//Probably run first kalman here, atleast make sure we run the predictor
	//one time before first filter.

	while(thread_args->run) {
		/* If regulator is off, jump to sleep */
		if (!regul->on)
			goto END_CLOCK;

		/* Control algorithm and write output */
		y_pitch = analogIn(0);
		y_yaw = analogIn(1);


		//Kalman filter and calc output
		if (regulator == REGULATOR_LQ) {
			Kalman(FILTER_TEN, y_pitch, y_yaw, 0, 0, states);
			//TODO calculate feedback using the matrix in simulink
			//Just implement by hand
			u_pitch = 0;
			u_yaw = 0;
		} else if (regulator == REGULATOR_MPC) {
			pthread_mutex_lock(regul->mutex);
			Kalman(FILTER_16, y_pitch, y_yaw, 0, 0, states);
			u_pitch = 0;
			u_yaw = 0;
			pthread_mutex_unlock(regul->mutex);
		}

		/* Push output*/
		u_pitch = limit(u_pitch); //Make u somewhere
		u_yaw= limit(u_yaw);
		analogOut(0, u_pitch);
		analogOut(1, u_yaw);

		/* Write to buffer. Lock buffer mutex */
		write_data(u_pitch, u_yaw, y_pitch, y_yaw, data);

		/*Kalman Predictor part*/
		if(regulator == REGULATOR_LQ) {
			Kalman(PREDICTOR_TEN, 0, 0, u_pitch, u_yaw, states);
		} else if (regulator== REGULATOR_MPC) {
			Kalman(PREDICTOR_16, 0, 0, u_pitch, u_yaw, states);
		}
		//TODO
		//Before switching regulator, the last cycle should use the
		//predictor for the mpc.

		/* Handle sleep */
		END_CLOCK: sleep_until(&ts);
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

int sleep_until(struct timespec *ts)
{
	ts->tv_nsec += h;
	if (ts->tv_nsec >= NS) {
		ts->tv_sec += 1;
		ts->tv_nsec -= NS;
	}

	if (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, ts, NULL))
		return -1;
	else
		return 0;
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
	data->mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(data->mutex, NULL);
	data->u_pitch = 0;
	data->u_yaw = 0;
	data->y_pitch = 0;
	data->y_yaw = 0;
	return data;
}

void free_data(data_t *data)
{
	pthread_mutex_destroy(data->mutex);
	free(data->mutex);
	free(data);
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
	pthread_mutex_lock(data->mutex);
	data->u_pitch = u_pitch;
	data->u_yaw = u_yaw;
	data->y_pitch = y_pitch;
	data->y_yaw = y_yaw;
	pthread_mutex_unlock(data->mutex);
}
