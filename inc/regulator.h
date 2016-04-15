#ifndef REGULATOR_H_
#define REGULATOR_H_

#include <stdlib.h>
#include <pthread.h>

typedef struct regul_t {
	int pitch_ref;
	int yaw_ref;
	pthread_mutex_t *mutex;
} regul_t;

float* limit(int u);

void* run_regul(regul_t *regul);

#endif /* REGULATOR_H_ */
