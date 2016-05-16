#include "regulator.h"
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "Kalman.h"

//#include "QPgen.h"
#include "data_struct.h"
#include "init_data.c"
#include "free_data.c"
#include "QPgen.c"


#define PREDICTOR_TEN (1)
#define FILTER_TEN (2)
#define PREDICTOR_16 (3)
#define FILTER_16 (4)

#define REGULATOR_LQ (0)
#define REGULATOR_MPC (1)

#define NS (1000000000l) /* Nanoseconds in a second */
#define h (50000000l) /* In nanoseconds, must be < 1 second */

#define NBR_OF_STATES (16)
#define HORIZON (30)


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
	void Kalman(char filter, double y_pitch, double y_yaw,
			double u_pitch, double u_yaw, double states[16]) {};
#else
#include "io.h"
#endif

void* run_regul(void *arg)
{
	thread_args_t *thread_args = (thread_args_t*) arg;
	regul_t *regul = thread_args->regul;
	data_t *data = thread_args->data;

	init();
	double states[16];
	char regulator = REGULATOR_LQ;

	/*Big rotor*/
	analogInOpen(0);
	analogOutOpen(0);

	/*Small rotor*/
	analogInOpen(1);
	analogOutOpen(1);

	struct timespec ts = { .tv_sec = time(NULL), .tv_nsec = 0 };

	double u_pitch, u_yaw, y_pitch, y_yaw, pitch_ref, yaw_ref;

/* QP gen stuff */
	struct DATA *qp_data = malloc(sizeof(struct DATA));
	double *solution = malloc(sizeof(double)*540);
	double *gt = malloc(sizeof(double)*540);
	int iter_val = 0;
	int *iter = &iter_val;
	
	init_data(qp_data);
	int controlmoves = 0;

	while(thread_args->run) {
		/* If regulator is off, jump to sleep */
		if (!regul->on) {
			analogOut(0, 0);
			analogOut(1, 0);
			write_data(u_pitch = 0, u_yaw = 0,
					y_pitch = 0, y_yaw = 0, data);
			goto END_CLOCK;
		}

		controlmoves += 1;

		if (controlmoves == 90) {
			regulator = REGULATOR_MPC;
			printf("SWITCHING TO MPC \n");
		}

		/* Control algorithm and write output */
		y_pitch = analogIn(0);
		y_yaw = analogIn(1);


		//Kalman filter and calc output
		if (regulator == REGULATOR_LQ) {
			Kalman(FILTER_TEN, y_pitch, y_yaw, 0, 0, states);
			calc_LQ(states, &u_pitch,&u_yaw);
		} else if (regulator == REGULATOR_MPC) {
			pthread_mutex_lock(regul->mutex);
			pitch_ref = regul->pitch_ref;
			yaw_ref = regul->yaw_ref;			
			Kalman(FILTER_16, y_pitch, y_yaw, 0, 0, states);
			calc_gt(gt,pitch_ref,yaw_ref);
			qp(qp_data,solution,iter,gt,states);
			//extract output
			u_pitch = solution[NBR_OF_STATES*HORIZON];
			u_yaw = solution[NBR_OF_STATES*HORIZON+1];
			printf("pith: %f, yaw: %f \n", u_pitch, u_yaw);
			printf("pithref: %f, yawref: %f \n", pitch_ref, yaw_ref);
			pthread_mutex_unlock(regul->mutex);
		}

		/* Push output*/
		u_pitch = u_pitch + 2.7;
		u_yaw = u_yaw + 4.3;
		u_pitch = limit(u_pitch);
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

	/*Free qp gen*/
	free_data(qp_data);

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

data_t *init_data_struct()
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

void free_data_struct(data_t *data)
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

void calc_LQ(double x[16], double *u1,double *u2)
{
	*u1 = -(166.1082*x[0]+62.2063*x[1]+140.5585*x[2]+43.7195*x[3]
		+ 115.9787*x[4] + 26.3260*x[5] + 92.7951*x[6] + 10.3428*x[7]
		+ 71.3845*x[8]  -3.9580*x[9]);
	*u2 = -(-10.8721*x[0]-86.5549*x[1]-9.8194*x[2]-83.6601*x[3]
		-8.4515*x[4] -80.4531*x[5] -6.8876*x[6] -77.0288*x[7]
		-5.2523*x[8]  -73.4754*x[9]);
}

void calc_gt(double in[540], double y1_ref, double y2_ref) {
	unsigned i;

	for (i = 0; i<HORIZON;++i) {
		in[i*NBR_OF_STATES] = -y1_ref*4;
		in[i*NBR_OF_STATES + 1] = -y2_ref;		 

	}
	for (i = 480;i!=HORIZON*18;i++) {
		in[i] = 0;
	}
}



 /*-[166.1082  -10.8721;
   62.2063  -86.5549;
  140.5585   -9.8194;
   43.7195  -83.6601;
  115.9787   -8.4515;
   26.3260  -80.4531;
   92.7951   -6.8876;
   10.3428  -77.0288;
   71.3845   -5.2523;
   -3.9580  -73.4754;zeros(6,2)]'*/
