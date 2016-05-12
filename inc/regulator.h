#ifndef REGULATOR_H_
#define REGULATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <pthread.h>

typedef struct regul_t {
	double pitch_ref;
	double yaw_ref;
	char on;
	pthread_mutex_t *mutex;
} regul_t;

typedef struct data_t {
	double u_yaw, u_pitch, y_yaw, y_pitch;
	pthread_mutex_t *mutex;
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

#ifdef NO_HARDWARE
	static void init() {};
	static void analogInOpen(int channel) {};
	static void analogOutOpen(int channel) {};
	static double analogIn(int channel) {return 0.0;};
	static void analogOut(int channel, double value) {};
	static void analogInClose(int channel) {};
	static void analogOutClose(int channel) {};
	static void Kalman(int filter, double y_pitch, double y_yaw,
			double u_pitch, double u_yaw, double states[16]) {};
#endif

#ifdef __cplusplus
}
#endif

#endif /* REGULATOR_H_ */
