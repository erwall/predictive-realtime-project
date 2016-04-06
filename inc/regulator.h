#ifndef REGULATOR_H_
#define REGULATOR_H_

#include <stdlib.h>
#include <pthread.h>

typedef struct regul_t {
	int pith_ref;
	int yaw_ref;
	pthread_mutex_t *mutex;
} regul_t;


void* run_regul(regul_t *regul)
{
	pthread_mutex_lock(regul->mutex);
	pthread_mutex_unlock(regul->mutex);
	return NULL;
}

#endif /* REGULATOR_H_ */
